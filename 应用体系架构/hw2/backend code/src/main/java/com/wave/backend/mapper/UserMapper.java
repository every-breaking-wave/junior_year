package com.wave.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.wave.backend.entity.User;
import org.apache.ibatis.annotations.Mapper;

@Mapper
public interface UserMapper extends BaseMapper<User> {
}