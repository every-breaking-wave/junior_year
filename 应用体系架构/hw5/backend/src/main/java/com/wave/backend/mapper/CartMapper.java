package com.wave.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.wave.backend.entity.Cart;
import org.apache.ibatis.annotations.Mapper;

/**
* @author Feng
* @description 针对表【car】的数据库操作Mapper
* @createDate 2022-05-15 13:49:40
* @Entity generator.domain.Car
*/
@Mapper
public interface CartMapper extends BaseMapper<Cart> {


}
