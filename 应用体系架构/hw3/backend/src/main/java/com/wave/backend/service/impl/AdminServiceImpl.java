package com.wave.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.wave.backend.dao.BookDao;
import com.wave.backend.dao.OrderDao;
import com.wave.backend.dao.UserDao;
import com.wave.backend.mapper.*;
import com.wave.backend.entity.Admin;
import com.wave.backend.entity.Book;
import com.wave.backend.entity.Order;
import com.wave.backend.entity.User;
import com.wave.backend.service.AdminService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.List;

/**
* @author Feng
* @description 针对表【admin(用户表)】的数据库操作Service实现
* @createDate 2022-05-09 17:46:17
*/
@Slf4j
@Service
public class AdminServiceImpl extends ServiceImpl<AdminMapper, Admin>
implements AdminService{
    @Resource
    private BookDao bookDao;

    @Resource
    private OrderDao orderDao;

    @Resource
    private UserDao userDao;

    @Override
    public void addBook(Book book) {
        // 判断是否同名
        if(bookDao.ifBookExist(book.getBookName())){
            return;
        }
        bookDao.saveOne(book);
        log.info("addBook Ok");
    }

    @Override
    public void delBook(Book book) {
        if(!bookDao.ifBookExist(book.getBookName())){
            return;
        }
        book.setIsDeleted(1);
        bookDao.saveOne(book);
        log.info("delete Ok");
    }

    @Override
    public void updateBook(Book book) {
        bookDao.saveOne(book);
    }

    @Override
    public boolean banUser(Integer userId) {
        User user;
        if( (user = userDao.findById(userId)) == null){
            log.info("Del user failed, user does not exist");
            return false;
        }
        user.setUserStatus(user.getUserStatus() == 1? 0 : 1);
        userDao.saveOne(user);
        log.info("ban user succeed");
        return true;
    }

    @Override
    public List<Order> getAllOrders() {
        return orderDao.getAll();
    }

    @Override
    public List<User> getAllUser() {
        return userDao.findAll();
    }
}
