package com.wave.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.wave.backend.constant.CreateOrderItemStatus;
import com.wave.backend.dao.BookDao;
import com.wave.backend.dao.OrderDao;
import com.wave.backend.dao.OrderItemDao;
import com.wave.backend.entity.Book;
import com.wave.backend.entity.CartItem;
import com.wave.backend.entity.OrderItem;
import  com.wave.backend.mapper.OrderitemMapper;
import com.wave.backend.model.response.CreateOrderItemResponse;
import com.wave.backend.service.OrderItemService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import java.util.List;

/**
* @author Feng
* @description 针对表【orderitem】的数据库操作Service实现
* @createDate 2022-05-13 15:13:14
*/
@Service
@Slf4j
public class OrderItemServiceImpl extends ServiceImpl<OrderitemMapper, OrderItem>
implements OrderItemService {

    @Resource
    private OrderDao orderDao;
    @Resource
    private OrderItemDao orderItemDao;
    @Resource
    private BookDao bookDao;

    @Override
    @Transactional
    public CreateOrderItemResponse createOrderItem(List<CartItem> bookInCarList, Integer orderId) {
        CreateOrderItemResponse createOrderItemResponse = new CreateOrderItemResponse();
        if( orderDao.getById(orderId) == null){
            createOrderItemResponse.setStatus(CreateOrderItemStatus.WRONG_ORDER_ID);
            return createOrderItemResponse;
        }

        System.out.println("订单列表如下:");
        // 遍历购物车，创建订单
        for (CartItem cartItem : bookInCarList) {
            Book book;
            if ((book = bookDao.findById(cartItem.getBookId()) ) == null) {
                createOrderItemResponse.setStatus(CreateOrderItemStatus.WRONG_ORDER_ID);
                return createOrderItemResponse;
            }
            OrderItem orderItem = new OrderItem();
            orderItem.setOrderId(orderId);
            orderItem.setBookId(cartItem.getBookId());
            orderItem.setPrice(book.getPrice());

            // 判断库存
            if(cartItem.getNumber() > book.getInventory()){
                createOrderItemResponse.setStatus(CreateOrderItemStatus.WRONG_NUM);
                return createOrderItemResponse;
            }
            //更新库存
            book.setInventory(book.getInventory() - cartItem.getNumber());
            bookDao.saveOne(book);
            orderItem.setNumber(cartItem.getNumber());

            orderItemDao.saveOne(orderItem);
            System.out.println(orderItem);
        }
        createOrderItemResponse.setStatus(CreateOrderItemStatus.ORDERITEM_ALL_OK);
        return createOrderItemResponse;

    }


}
