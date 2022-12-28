package com.wave.backend.controller;



import com.wave.backend.entity.Order;
import com.wave.backend.model.request.CreateOrderRequest;
import com.wave.backend.model.response.CreateOrderResponse;
import com.wave.backend.service.OrderService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.kafka.core.KafkaTemplate;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.util.List;

@Slf4j
@CrossOrigin
@RestController
@RequestMapping("/order")
public class OrderController {

    @Resource
    private OrderService orderService;

    @Autowired
    private KafkaTemplate<String, String> kafkaTemplate;

//    @PostMapping("/create")
//    public CreateOrderResponse createOrder(@RequestBody CreateOrderRequest createOrderRequest) {
//
//        if (createOrderRequest == null)
//            return null;
//
//        Integer userId =  createOrderRequest.getUserId();
//        return orderService.createOrder(userId);
//    }

    @PostMapping("/get/{userId}")
    public List<Order> getOrdersById(@PathVariable Integer userId){
        return orderService.getOrdersById(userId);
    }

    @PostMapping("/search/{keyword}")
    public List<Order> searchOrderItems(@PathVariable String keyword, HttpServletRequest request){
        return orderService.searchOrders(keyword, request);
    }

    @PostMapping("/user-search/{keyword}")
    public List<Order> searchUserOrderItems(@PathVariable String keyword, HttpServletRequest request){
        return orderService.searchOrders(keyword, request);
    }

    @PostMapping("/get-user-full-order/{userId}")
    public List<Order>getUserFullOrderItems(@PathVariable Integer userId, HttpServletRequest request){
        return orderService.getUserFullOrders(userId);
    }

    @PostMapping("/create")
    public void send(@RequestBody CreateOrderRequest createOrderRequest){
        kafkaTemplate.send("topic1", "key", createOrderRequest.getUserId().toString());
    }



}
