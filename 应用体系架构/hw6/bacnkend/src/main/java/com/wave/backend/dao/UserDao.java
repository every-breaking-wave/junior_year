package com.wave.backend.dao;

import com.wave.backend.entity.User;

import java.util.List;


public interface UserDao {

    Long getCount(String userAccount);

    Integer saveOne(User user);

    User findByAccountAndPassword(String userAccount, String userPassword);

    User findById(Integer userId);

    List<User> findAll();
}
