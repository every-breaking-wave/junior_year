package com.wave.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.wave.backend.entity.CartItem;
import org.apache.ibatis.annotations.Mapper;

/**
* @author Feng
* @description 针对表【caritem】的数据库操作Mapper
* @createDate 2022-05-15 13:49:47
* @Entity generator.domain.Caritem
*/
@Mapper
public interface CartItemMapper extends BaseMapper<CartItem> {


}
