package com.wave.backend.dao;

import com.wave.backend.entity.CartItem;

import java.util.List;

public interface CartItemDao {
    void deleteOne(CartItem cartItem);

    CartItem findByBookIdAndUserId(Integer bookId, Integer userId);

    Integer saveOne(CartItem cartItem);

    CartItem getVo(CartItem cartItem);

    List<CartItem> getVos(List<CartItem> cartItems);
}
