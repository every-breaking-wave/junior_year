package com.wave.backend.service;
import com.baomidou.mybatisplus.extension.service.IService;
import com.wave.backend.entity.OrderItem;
import com.wave.backend.entity.User;
import com.wave.backend.model.response.UserLoginResponse;
import com.wave.backend.model.response.UserRegisterResponse;

import javax.servlet.http.HttpServletRequest;

/**
* @author 16541
* @description 针对表【user(用户表)】的数据库操作Service
* @createDate 2022-04-12 22:46:44
*/


public interface UserService extends IService<User>  {

    /**
     * @param userAccount    :用户账号
     * @param userPassword   ：用户密码
     * @param repeatPassword
     * @param email
     * @return 生成的用户ID
     */
    UserRegisterResponse userRegister(String userAccount, String userPassword, String repeatPassword, String email);
    UserLoginResponse userLogin(String userAccount, String userPassword, HttpServletRequest request);

    /**
     *
     * @param originUser: 原用户
     * @return 脱敏后的用户
     */
    User getSaveUser(User originUser);

    Double userLogout(HttpServletRequest request);

    Double timeCounter(HttpServletRequest request, Boolean isStart);

    Boolean isAdmin(HttpServletRequest request);
}
