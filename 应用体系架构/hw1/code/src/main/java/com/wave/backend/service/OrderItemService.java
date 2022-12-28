package com.wave.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.wave.backend.entity.CartItem;
import com.wave.backend.entity.OrderItem;
import com.wave.backend.model.response.CreateOrderItemResponse;

import java.util.List;

/**
* @author Feng
* @description 针对表【orderitem】的数据库操作Service
* @createDate 2022-05-13 15:13:14
*/
public interface OrderItemService extends IService<OrderItem> {

    CreateOrderItemResponse createOrderItem(List<CartItem> bookInCarList, Integer orderId);



}
