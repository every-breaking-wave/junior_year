package com.wave.backend.dao.daoImpl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.github.dreamyoung.mprelation.AutoMapper;
import com.wave.backend.dao.OrderItemDao;
import com.wave.backend.mapper.OrderitemMapper;
import com.wave.backend.entity.OrderItem;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import java.util.List;

@Component
public class OrderItemDaoImpl implements OrderItemDao {

    @Resource
    private OrderitemMapper orderitemMapper;

    @Resource
    private AutoMapper autoMapper;

    @Override
    public Integer saveOne(OrderItem orderItem) {
        if(orderItem.getId() == null){
            return orderitemMapper.insert(orderItem);
        }
        return orderitemMapper.updateById(orderItem);
    }

    @Override
    @Transactional(propagation = Propagation.REQUIRES_NEW)
    public void saveList(List<OrderItem> orderItems) {
        for (OrderItem orderItem: orderItems){
            saveOne(orderItem);
        }
        Integer result = 10 / 0;
    }

    @Override
    public List<OrderItem> getAll() {
        return orderitemMapper.selectList(new QueryWrapper<>());
    }

    @Override
    public List<OrderItem> findAllByUserId(Integer userId) {
        QueryWrapper<OrderItem> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("userId", userId);
        return orderitemMapper.selectList(queryWrapper);
    }

    @Override
    public OrderItem getVo(OrderItem orderItem) {
        return autoMapper.mapperEntity(orderItem);
    }

}
