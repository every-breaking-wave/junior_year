package com.wave.backend;

import com.github.dreamyoung.mprelation.AutoMapper;
import com.wave.backend.mapper.OrderMapper;
import com.wave.backend.entity.Order;
import com.wave.backend.service.OrderService;
import lombok.extern.slf4j.Slf4j;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;

import javax.annotation.Resource;

@SpringBootTest
@Slf4j
class BackendApplicationTests {

    @Resource
    private OrderService orderService;
    @Resource
    private OrderMapper orderMapper;
    @Resource
    private AutoMapper autoMapper;

    @Test
    void test() {
        Order order = orderMapper.selectById(177414145);
        autoMapper.mapperEntity(order);
        log.info(order.toString());
        System.out.println(order);
        log.info(order.getOrderItems().toString());
    }

}
