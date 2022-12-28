package com.wave.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.wave.backend.constant.CreateOrderStatus;
import com.wave.backend.dao.BookDao;
import com.wave.backend.dao.OrderDao;
import com.wave.backend.dao.OrderItemDao;
import com.wave.backend.dao.UserDao;
import com.wave.backend.entity.*;
import  com.wave.backend.mapper.OrderMapper;
import com.wave.backend.model.response.CreateOrderResponse;
import com.wave.backend.service.CartService;
import  com.wave.backend.service.OrderService;
import com.wave.backend.service.UserService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.util.ArrayList;
import java.util.List;

import static com.wave.backend.constant.UserConstant.USER_ID;
import static com.wave.backend.constant.UserConstant.USER_LOGIN_STATE;

/**
* @author Feng
* @description 针对表【order】的数据库操作Service实现
* @createDate 2022-05-13 15:13:11
*/
@Service
@Slf4j
public class OrderServiceImpl extends ServiceImpl<OrderMapper, Order>
implements OrderService{

    @Resource
    private UserDao userDao;
    @Resource
    private OrderDao orderDao;
    @Resource
    private OrderItemDao orderItemDao;
    @Resource
    private BookDao bookDao;

    @Resource
    private CartService cartService;

    @Resource
    private UserService userService;


    @Override
    @Transactional
    public CreateOrderResponse createOrder(Integer userId) {
        log.info("Creating Order");

        CreateOrderResponse createOrderResponse = new CreateOrderResponse();

        User user = userDao.findById(userId);
        // 查询 UserId 是否存在
        if (user == null) {
            createOrderResponse.setStatus(CreateOrderStatus.WRONG_USER_ID);
            return createOrderResponse;
        }

        Order order = new Order();
        Double price = 0.0;
        order.setUserId(user.getId());

        // 调用的第一个 Transaction 函数
        orderDao.saveOne(order);
        Integer orderId = order.getId();

        List<CartItem> bookInCarList = cartService.getCarByUserId(userId).getCartItems();
        List<OrderItem> orderItems = new ArrayList<>();

        System.out.println("订单列表如下:");
        // 遍历购物车，创建订单
        for (CartItem cartItem : bookInCarList) {
            Book book;
            if ((book = bookDao.findById(cartItem.getBookId()) ) == null) {
                createOrderResponse.setStatus(CreateOrderStatus.WRONG_BOOK_ID);
                return createOrderResponse;
            }
            OrderItem orderItem = new OrderItem();
            orderItem.setOrderId(orderId);
            orderItem.setBookId(cartItem.getBookId());
            orderItem.setPrice(book.getPrice());

            // 判断库存
            if(cartItem.getNumber() > book.getInventory()){
                createOrderResponse.setStatus(CreateOrderStatus.NO_ENOUGH_INVENTORY);
                return createOrderResponse;
            }
            //更新库存
            book.setInventory(book.getInventory() - cartItem.getNumber());
            bookDao.saveOne(book);
            orderItem.setNumber(cartItem.getNumber());
            orderItems.add(orderItem);
            System.out.println(orderItem);
            price += orderItem.getPrice().doubleValue() * cartItem.getNumber();
        }

        // 调用的第二个 Transaction 函数
        try {
            orderItemDao.saveList(orderItems);
        } catch (RuntimeException e) {
            e.printStackTrace();
        }

        createOrderResponse.setStatus(CreateOrderStatus.ORDER_ALL_OK);
        createOrderResponse.setId(orderId);
        createOrderResponse.setUserId(userId);
        createOrderResponse.setPrice(price);

        log.info("created order successfully");
        //清空购物车
        cartService.clearCar(userId);
        return createOrderResponse;
    }

    @Override
    public boolean delOrder(Integer userId) {

//        Order order;
//        if(order = orderDao. orderMapper.selectById(userId) != null){
//            log.error("no such user");
//            return false;
//        }
//
//        QueryWrapper<Order> queryWrapper = new QueryWrapper<>();
//        queryWrapper.eq("userId", userId);
//        Order order = orderMapper.selectOne(queryWrapper);
//        QueryWrapper<OrderItem> queryWrapper1 = new QueryWrapper<>();
//        queryWrapper1.eq("orderId", order.getId());
//        orderitemMapper.delete(queryWrapper1);
//        orderMapper.delete(queryWrapper);
//        log.info("del Order !");
//        return true;
        return true;
    }



    @Override
    public List<Order> getOrdersById(Integer userId) {
        List<Order> orders = orderDao.findAllByUserId(userId);
        return orderDao.getVos(orders);
    }


    @Override
    //TODO : 搜索过滤
    public List<Order> searchOrders(String keyword, HttpServletRequest request) {
        log.info("Searching orders");
        List<Order> Orders;
        if(!userService.isAdmin(request)){
            Object o = request.getSession().getAttribute(USER_ID);
            Integer userId = (Integer) o;
            Orders = getOrdersById(userId);
        }
        else Orders =  getFullOrders();
        if(keyword.equals("default")){
            return Orders;
        }
        // 筛选出需要的订单 items
        for (int i = 0; i < Orders.size(); i++){
            for (int j = 0; j < Orders.get(i).getOrderItems().size(); j++){
                if (!Orders.get(i).getOrderItems().get(j).getBook().getBookName().contains(keyword) ){
                    Orders.get(i).getOrderItems().remove(j);
                    j--;
                }
            }
            if(Orders.get(i).getOrderItems().size() == 0){
                Orders.remove(i);
                i--;
            }
        }
        log.info(Orders.toString());
        log.info("get search orderItems");
        return Orders;
    }

    @Override
    public List<Order> searchUserOrders(String keyword, HttpServletRequest request) {
        log.info("Searching orders");

        Object userObj = request.getSession().getAttribute(USER_LOGIN_STATE);
        User user = (User) userObj;
        List<Order> Orders = getOrdersById(user.getId());
        if(keyword.equals("default")){
            return Orders;
        }
        // 筛选出需要的订单 items
        for (int i = 0; i < Orders.size(); i++){
            for (int j = 0; j < Orders.get(i).getOrderItems().size(); j++){
                if (!Orders.get(i).getOrderItems().get(j).getBook().getBookName().contains(keyword) ){
                    Orders.get(i).getOrderItems().remove(j);
                    j--;
                }
            }
            if(Orders.get(i).getOrderItems().size() == 0){
                Orders.remove(i);
                i--;
            }
        }
        log.info(Orders.toString());
        log.info("get search orderItems");
        return Orders;
    }

    @Override
    public List<Order> getFullOrders() {
        List<Order> orders = orderDao.getVos(orderDao.getAll());
        log.info("get full orderItems:\n");
        log.info(orders.toString());
        return orders;
    }

    @Override
    public List<Order> getUserFullOrders(Integer userId) {
        List<Order> orders = orderDao.findAllByUserId(userId);
        return orderDao.getVos(orders);
    }
}




