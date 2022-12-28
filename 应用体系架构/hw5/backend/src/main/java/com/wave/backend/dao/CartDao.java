package com.wave.backend.dao;

import com.wave.backend.entity.Cart;



public interface CartDao {
    Integer saveOne(Cart cart);

    Cart findByUserId(Integer userId);

    Cart getVo(Cart cart);
}
