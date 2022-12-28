package com.wave.backend.dao;

import com.wave.backend.entity.Admin;


public interface AdminDao {
    Admin findByAccountAndPassword(String userAccount, String userPassword);

    Integer saveOne(Admin admin);

    Boolean ifExist(Admin admin);
}
