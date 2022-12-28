package com.wave.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.wave.backend.entity.Admin;
import org.apache.ibatis.annotations.Mapper;

/**
* @author Feng
* @description 针对表【admin(用户表)】的数据库操作Mapper
* @createDate 2022-05-09 17:46:17
* @Entity generator.domain.Admin
*/
@Mapper
public interface AdminMapper extends BaseMapper<Admin> {


}
