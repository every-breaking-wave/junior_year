package com.wave.backend.dao.daoImpl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.wave.backend.dao.UserDao;
import com.wave.backend.mapper.UserMapper;
import com.wave.backend.entity.User;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.List;

@Component
public class UserDaoImpl implements UserDao {
    @Resource
    UserMapper userMapper;

    @Override
    public Long getCount(String userAccount) {
        QueryWrapper<User> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("userAccount", userAccount);
        Long count = userMapper.selectCount(queryWrapper);
        return count;
    }

    @Override
    public Integer saveOne(User user) {
        if (user.getId() != null) {
            return userMapper.updateById(user);
        }
        return userMapper.insert(user);
    }

    @Override
    public User findByAccountAndPassword(String userAccount, String userPassword) {
        QueryWrapper<User> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("userAccount", userAccount);
        queryWrapper.eq("userPassword", userPassword);
        return userMapper.selectOne(queryWrapper);
    }

    @Override
    public User findById(Integer userId) {
        return userMapper.selectById(userId);
    }

    @Override
    public List<User> findAll() {
        return userMapper.selectList(new QueryWrapper<>());
    }
}
