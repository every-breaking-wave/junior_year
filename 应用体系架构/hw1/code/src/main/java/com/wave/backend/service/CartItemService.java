package com.wave.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.wave.backend.entity.CartItem;

import java.util.List;

/**
* @author Feng
* @description 针对表【caritem】的数据库操作Service
* @createDate 2022-05-15 13:49:47
*/
public interface CartItemService extends IService<CartItem> {


    Boolean addByCount(Integer userId, Integer bookId, Integer count);

    Boolean minByCount(Integer userId, Integer bookId, Integer count);

    Boolean delOne(Integer userId, Integer bookId, Integer count);

    List<CartItem> getVos(List<CartItem> cartItems);
}
