package com.wave.backend.dao;

import com.wave.backend.entity.Order;

import java.util.List;

public interface OrderDao {

    void saveOne(Order order);

    List<Order> getAll();

    List<Order> findAllByUserId(Integer userId);

    Order getVo(Order order);

    List<Order> getVos(List<Order> orders);

    Order getById(Integer orderId);
}
