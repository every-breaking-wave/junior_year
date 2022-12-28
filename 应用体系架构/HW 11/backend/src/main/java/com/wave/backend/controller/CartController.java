package com.wave.backend.controller;

import com.wave.backend.entity.Cart;
import com.wave.backend.entity.CartItem;
import com.wave.backend.model.request.updateCartItemRequest;
import com.wave.backend.model.request.CartListRequest;
import com.wave.backend.model.response.CarListResponse;
import com.wave.backend.service.CartItemService;
import com.wave.backend.service.CartService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.UnsupportedEncodingException;
import java.util.*;

@RestController
@Slf4j
@RequestMapping("/cart")
public class CartController {

    @Resource
    private CartService cartService;
    @Resource
    private CartItemService cartItemService;

    @RequestMapping("/getId")
    public Integer getId(@RequestBody Integer userId){
        Cart cart =  cartService.getCarByUserId(userId);
        return cart.getId();
    }

    /**
     * 添加一个商品到购物车
     */
    @RequestMapping("/addCart")
    public Boolean addCartItem(@RequestBody updateCartItemRequest updateCartItemRequest, HttpServletRequest request, HttpServletResponse response) throws Exception {
        Integer userId = updateCartItemRequest.getUserId();
        Integer bookId = updateCartItemRequest.getBookId();
        Integer count = updateCartItemRequest.getCount();

        return cartItemService.addByCount(userId, bookId, count);
    }

    /**
     * 减少一个商品到购物车
     */
    @RequestMapping("/minCart")
    public Boolean minCartItem(@RequestBody updateCartItemRequest updateCartItemRequest, HttpServletRequest request, HttpServletResponse response) throws Exception {
        Integer userId = updateCartItemRequest.getUserId();
        Integer bookId = updateCartItemRequest.getBookId();
        Integer count = updateCartItemRequest.getCount();

        return cartItemService.minByCount(userId, bookId, count);
    }


    @RequestMapping("/delCart")
    public Boolean delCartItem(@RequestBody updateCartItemRequest updateCartItemRequest, HttpServletRequest request, HttpServletResponse response) throws Exception {
        Integer userId = updateCartItemRequest.getUserId();
        Integer bookId = updateCartItemRequest.getBookId();
        Integer count = updateCartItemRequest.getCount();

        return cartItemService.delOne(userId, bookId, count);
    }



    @RequestMapping("/changeCart")
    public String changeCart(@RequestBody updateCartItemRequest updateCartItemRequest, HttpServletRequest request, HttpServletResponse response) throws Exception {
//        QueryWrapper<CartItem> queryWrapper = new QueryWrapper<>();
//        QueryWrapper<Cart> queryWrapper1 = new QueryWrapper<>();
//
//        queryWrapper1.eq("userId", updateCartItemRequest.getUserId());
//        Cart cart = cartMapper.selectOne(queryWrapper1);
//
//        queryWrapper.eq("cartId", cart.getId());
//        queryWrapper.eq("bookId", updateCartItemRequest.getBookId());
//        CartItem cartItem = cartItemMapper.selectOne(queryWrapper);
//
//        Integer count = updateCartItemRequest.getCount();
//        if(count == 0){
//           cartItemMapper.deleteById(cartItem);
//           return "delete ok";
//        }
//        cartItem.setNumber(count);
//        cartItemMapper.updateById(cartItem);
//        return "ok";
        return null;
    }

    /**
     * 购物车商品列表
     */
    @RequestMapping("/cartList")
    public CarListResponse cartList(@RequestBody CartListRequest cartListRequest) throws UnsupportedEncodingException {

        Cart cart = cartService.getCarByUserId(cartListRequest.getUserId());
        List<CartItem> cartItems = cart.getCartItems();
        CarListResponse carListResponse = new CarListResponse();

        cartItems = cartItemService.getVos(cartItems);

        System.out.println("购物车列表如下:");
        for (CartItem cartItem : cartItems) {
            System.out.println(cartItem);
        }

        carListResponse.setBookInCarList(cartItems);

        return carListResponse;
    }


}

