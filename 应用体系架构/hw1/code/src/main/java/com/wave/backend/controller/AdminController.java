package com.wave.backend.controller;


import com.wave.backend.entity.Book;
import com.wave.backend.entity.Order;
import com.wave.backend.entity.User;
import com.wave.backend.model.request.CreateOrderRequest;
import com.wave.backend.service.AdminService;
import com.wave.backend.service.OrderService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.util.List;

@Slf4j
@CrossOrigin
@RestController
@RequestMapping("/admin")
public class AdminController {
    @Resource
    private UserController userController;
    @Resource
    private AdminService adminService;
    @Resource
    private OrderService orderService;

    @PostMapping("/add-book")
    public void addBook(@RequestBody Book book, HttpServletRequest request){
        if(userController.isAdmin(request))
            adminService.addBook(book);
    }

    @PostMapping("/del-book")
    public void delBook(@RequestBody Book book, HttpServletRequest request){
        if(userController.isAdmin(request))
            adminService.delBook(book);
    }
    @PostMapping("/update-book")
    public void updateBook(@RequestBody Book book, HttpServletRequest request){
        if(userController.isAdmin(request))
            adminService.updateBook(book);
    }
    @PostMapping("/ban-user")
    public boolean banUser(@RequestBody Integer userId,  HttpServletRequest request){
            return userController.isAdmin(request) && adminService.banUser(userId);
    }

    @PostMapping("/get-all-order")
    public List<Order> getAllOrders(HttpServletRequest request){
        if(userController.isAdmin(request))
            return adminService.getAllOrders();
        return null;
    }

    @PostMapping("/get-all-user")
    public List<User> getAllUsers(HttpServletRequest request){
        if(userController.isAdmin(request))
            return adminService.getAllUser();
        log.error("normal user want to access admin page! has rejected");
        return null;
    }

    @PostMapping("/delete")
    public boolean delOrder(@RequestBody CreateOrderRequest createOrderRequest) {
        if (createOrderRequest == null)
            return false;
        return orderService.delOrder(createOrderRequest.getUserId());
    }

    @PostMapping("/get-full-order")
    public List<Order> getFullOrderItems(HttpServletRequest request){
        if(userController.isAdmin(request))
            return orderService.getFullOrders();
        return null;
    }






}
