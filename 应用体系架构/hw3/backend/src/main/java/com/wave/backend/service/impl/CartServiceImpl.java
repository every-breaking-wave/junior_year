package com.wave.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.wave.backend.dao.CartDao;
import com.wave.backend.dao.CartItemDao;
import com.wave.backend.entity.Cart;
import com.wave.backend.mapper.CartMapper;
import com.wave.backend.entity.CartItem;
import com.wave.backend.service.CartService;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;

/**
 * @author Feng
 * @description 针对表【car】的数据库操作Service实现
 * @createDate 2022-05-15 13:49:40
 */
@Service
public class CartServiceImpl extends ServiceImpl<CartMapper, Cart>
        implements CartService {
    @Resource
    private CartDao cartDao;
    @Resource
    private CartItemDao cartItemDao;


    @Override
    public Cart getCarByUserId(Integer userId) {
        return cartDao.getVo(cartDao.findByUserId(userId));
    }

    // TODO: 得验证是否正常删除
    @Override
    public void clearCar(Integer userId) {
        Cart cart = cartDao.getVo(cartDao.findByUserId(userId));
        for(CartItem cartItem : cart.getCartItems()){
            cartItemDao.deleteOne(cartItem);
        }
        cartDao.saveOne(cart);
    }
}
