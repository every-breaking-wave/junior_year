package com.wave.backend.service.impl;

import com.github.dreamyoung.mprelation.ServiceImpl;
import com.wave.backend.constant.UserConstant;
import com.wave.backend.constant.UserServiceStatus;
import com.wave.backend.dao.AdminDao;
import com.wave.backend.dao.CartDao;
import com.wave.backend.dao.UserDao;
import com.wave.backend.entity.Admin;
import com.wave.backend.entity.Cart;
import com.wave.backend.entity.User;
import com.wave.backend.mapper.UserMapper;
import com.wave.backend.model.response.UserLoginResponse;
import com.wave.backend.model.response.UserRegisterResponse;
import com.wave.backend.service.UserService;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.stereotype.Service;
import org.springframework.util.DigestUtils;
import org.springframework.web.context.annotation.SessionScope;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.wave.backend.constant.UserConstant.ADMIN_ROLE;
import static com.wave.backend.constant.UserConstant.USER_LOGIN_STATE;

/**
 * 用户服务实现类
 *
 * @author example
 */

@Service
@Slf4j
@SessionScope
public class UserServiceImpl extends ServiceImpl<UserMapper, User>
        implements UserService{
    /**
     * 盐值： 用于混淆密码
     */
    private static final String SALT = "wave";

    @Resource
    private UserDao userDao;

    @Resource
    private AdminDao adminDao;

    @Resource
    private CartDao cartDao;

    @Override
    public UserRegisterResponse userRegister(String userAccount, String userPassword, String repeatPassword, String email) {

        log.info("Registering");

        UserRegisterResponse userRegisterResponse = new UserRegisterResponse();

        // 1.校验
        if (StringUtils.isAnyBlank(userAccount, userPassword, repeatPassword, email)) {
            userRegisterResponse.setStatus(UserServiceStatus.USER_ACCOUNT_PASSWORD_NULL);
            return userRegisterResponse;
        }

        // 两个密码是否匹配
        if (!userPassword.equals(repeatPassword)) {
            userRegisterResponse.setStatus(UserServiceStatus.PASSWORD_NOT_MATE_REPEAT_PASSWORD);
            return userRegisterResponse;
        }

        // 判断邮箱格式
        String regEx1 = "^([a-z0-9A-Z]+[-|\\.]?)+[a-z0-9A-Z]@([a-z0-9A-Z]+(-[a-z0-9A-Z]+)?\\.)+[a-zA-Z]{2,}$";
        Pattern p;
        Matcher m;
        p = Pattern.compile(regEx1);
        m = p.matcher(email);
        if (!m.matches()) {
            userRegisterResponse.setStatus(UserServiceStatus.USER_EMAIL_ILLEGAL);
            return userRegisterResponse;
        }

        /*
         * 账户不能包含特殊字
         * \\pP表示标点符号
         * \\pS表示特殊符号（数学符号，货币符号等）
         * \\s+表示一或多个空格
         */

        String validPattern = "\\pP|\\pS|\\s+";
        Matcher matcher = Pattern.compile(validPattern).matcher(userAccount);
        if (matcher.find()) {
            userRegisterResponse.setStatus(UserServiceStatus.USER_ACCOUNT_ILLEGAL);
            return userRegisterResponse;
        }

        if (userDao.getCount(userAccount) > 0) {
            userRegisterResponse.setStatus(UserServiceStatus.USER_ALREADY_EXIST);
            return userRegisterResponse;
        }

        String encryptPassword = DigestUtils.md5DigestAsHex((SALT + userPassword).getBytes());
        User user = new User();
        user.setUserAccount(userAccount);
        user.setUserPassword(encryptPassword);
        user.setEmail(email);
        int saveResult = userDao.saveOne(user);
        if (saveResult == 0) {
            userRegisterResponse.setStatus(UserServiceStatus.USER_UNKNOWN_ERROR);
            return userRegisterResponse;
        }
        // 初始化购物车
        Cart cart = new Cart();
        cart.setUserId(user.getId());
        cartDao.saveOne(cart);

        userRegisterResponse.setStatus(UserServiceStatus.USER_ALL_OK);
        userRegisterResponse.setId(0L);
        return userRegisterResponse;
    }


    @Override
    public UserLoginResponse userLogin(String userAccount, String userPassword, HttpServletRequest request) {
        log.info("Logging in");

        UserLoginResponse userLoginResponse = new UserLoginResponse();

        // 1.校验
        if (StringUtils.isAnyBlank(userAccount, userPassword)) {
            userLoginResponse.setStatus(UserServiceStatus.USER_ACCOUNT_PASSWORD_NULL);
            return userLoginResponse;
        }

        // 账户不能包含特殊字符
        String validPattern = "\\p{P}|\\p{S}|\\s+";
        Matcher matcher = Pattern.compile(validPattern).matcher(userAccount);
        if (matcher.find()) {
            userLoginResponse.setStatus(UserServiceStatus.USER_ACCOUNT_ILLEGAL);
            return userLoginResponse;
        }

        // 2.加密
        String encryptPassword = DigestUtils.md5DigestAsHex((SALT + userPassword).getBytes());
        // 查询用户是否存在并判断role

        User user = userDao.findByAccountAndPassword(userAccount,encryptPassword);
        Admin admin = adminDao.findByAccountAndPassword(userAccount, encryptPassword);
       
        // 用户不存在
        if (user == null && admin == null) {
            log.error("User login failed: User doesn't exist.");
            userLoginResponse.setStatus(UserServiceStatus.USER_NOT_EXIST);
            return userLoginResponse;
        }

        // 判断用户是否被禁用
        if (user != null && user.getUserStatus() == 1){
            log.error("User login failed: User has been banned.");
            userLoginResponse.setStatus(UserServiceStatus.USER_BEEN_BANNED);
            return userLoginResponse;
        }

        // 3.用户脱敏

        // 4.记录脱敏后的用户登录状态, 现在未进行脱敏处理，按用户role分类记录用户态
        if (user != null) {
            request.getSession().setAttribute(UserConstant.USER_LOGIN_STATE, user.getUserRole());
            request.getSession().setAttribute(UserConstant.USER_ID, user.getId());
            userLoginResponse.setId(user.getId());
            userLoginResponse.setUserAccount(user.getUserAccount());
            userLoginResponse.setRole(UserConstant.DEFAULT_ROLE);
        }
        else {
            request.getSession().setAttribute(UserConstant.USER_LOGIN_STATE, admin.getUserRole());
            userLoginResponse.setId(admin.getId());
            userLoginResponse.setUserAccount(admin.getUserAccount());
            userLoginResponse.setRole(UserConstant.ADMIN_ROLE);
        }

        userLoginResponse.setStatus(UserServiceStatus.USER_ALL_OK);

        // 开始计时
        timeCounter(request, true);

        return userLoginResponse;
    }

    @Override
    public User getSaveUser(User originUser) {
        User safeUser = new User();
        safeUser.setId(0);
        safeUser.setUserName(originUser.getUserName());
        safeUser.setUserAccount(originUser.getUserAccount());
        safeUser.setAvatarUrl(originUser.getAvatarUrl());
        safeUser.setGender(originUser.getGender());
        safeUser.setEmail(originUser.getEmail());
        safeUser.setUserStatus(originUser.getUserStatus());
        safeUser.setCreateTime(originUser.getCreateTime());
        safeUser.setIsDelete(0);
        safeUser.setUserRole(originUser.getUserRole());
        return safeUser;
    }

    @Override
    public Double userLogout(HttpServletRequest request)  {
        // 移除登录态
        request.getSession().removeAttribute(USER_LOGIN_STATE);
        return timeCounter(request, false);
    }

    @Override
    public Double timeCounter(HttpServletRequest request, Boolean isStart) {
        if(isStart) {
            request.getSession().setAttribute("time", System.currentTimeMillis());
            return 0.0;
        }
        Double time = ((Long)(System.currentTimeMillis() - (Long) request.getSession().getAttribute("time"))).doubleValue() / 1000;
        log.info( "Login time is :" + time +"s");
        return time;
    }

    @Override
    public Boolean isAdmin(HttpServletRequest request) {
        Object obj = request.getSession().getAttribute(USER_LOGIN_STATE);
        Integer role = (Integer) obj;
        return role == ADMIN_ROLE; // 返回空数组
    }
}