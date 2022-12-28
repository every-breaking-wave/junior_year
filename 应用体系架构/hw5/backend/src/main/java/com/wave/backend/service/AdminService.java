package com.wave.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.wave.backend.entity.Admin;
import com.wave.backend.entity.Book;
import com.wave.backend.entity.Order;
import com.wave.backend.entity.User;

import java.util.List;

/**
* @author Feng
* @description 针对表【admin(用户表)】的数据库操作Service
* @createDate 2022-05-09 17:46:17
*/
public interface AdminService extends IService<Admin> {
    void addBook(Book book);

    void delBook(Book book);

    void updateBook(Book book);

    boolean banUser(Integer userId);

    List<Order> getAllOrders();

    List<User> getAllUser();
}
