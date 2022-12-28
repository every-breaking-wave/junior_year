package com.wave.backend.dao.daoImpl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.wave.backend.dao.AdminDao;
import com.wave.backend.mapper.AdminMapper;
import com.wave.backend.entity.Admin;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;

@Component
public class AdminDaoImpl implements AdminDao {
    @Resource
    private AdminMapper adminMapper;


    @Override
    public Admin findByAccountAndPassword(String userAccount, String userPassword) {
        QueryWrapper<Admin> queryWrapper1 = new QueryWrapper<>();
        queryWrapper1.eq("userAccount", userAccount);
        queryWrapper1.eq("userPassword", userPassword);
        return adminMapper.selectOne(queryWrapper1);
    }

    @Override
    public Integer saveOne(Admin admin) {
        return adminMapper.updateById(admin);
    }

    @Override
    public Boolean ifExist(Admin admin) {
        return adminMapper.selectById(admin) != null;
    }

}
