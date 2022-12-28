package com.wave.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.wave.backend.entity.OrderItem;
import org.apache.ibatis.annotations.Mapper;

/**
* @author Feng
* @description 针对表【orderitem】的数据库操作Mapper
* @createDate 2022-05-13 15:13:14
* @Entity generator.domain.Orderitem
*/
@Mapper
public interface OrderitemMapper extends BaseMapper<OrderItem> {

}
