package com.wave.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.wave.backend.entity.Cart;

/**
* @author Feng
* @description 针对表【car】的数据库操作Service
* @createDate 2022-05-15 13:49:40
*/
public interface CartService extends IService<Cart> {

    Cart getCarByUserId(Integer userId);

    void clearCar(Integer userId);
}
