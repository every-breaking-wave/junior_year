package com.wave.backend.dao;

import com.wave.backend.entity.OrderItem;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

@Transactional
public interface OrderItemDao {

    Integer saveOne(OrderItem orderItem);

    void  saveList(List<OrderItem> orderItems);

    List<OrderItem> getAll();

    List<OrderItem> findAllByUserId(Integer userId);

    OrderItem getVo(OrderItem orderItem);

}
