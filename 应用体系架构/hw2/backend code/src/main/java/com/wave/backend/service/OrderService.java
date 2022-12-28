package com.wave.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;

import com.wave.backend.entity.Order;
import com.wave.backend.model.response.CreateOrderResponse;

import javax.servlet.http.HttpServletRequest;
import java.util.List;


/**
* @author Feng
* @description 针对表【order】的数据库操作Service
* @createDate 2022-05-13 15:13:11
*/
public interface OrderService extends IService<Order> {

    CreateOrderResponse createOrder(Integer userId);

    boolean delOrder(Integer userId);

    List<Order> getOrdersById(Integer userId);

    List<Order> getFullOrders();

    List<Order> getUserFullOrders(Integer userId);

    List<Order> searchOrders(String keyword, HttpServletRequest request);

    List<Order> searchUserOrders(String keyword, HttpServletRequest request);

}
