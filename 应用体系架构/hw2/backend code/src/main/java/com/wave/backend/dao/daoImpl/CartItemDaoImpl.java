package com.wave.backend.dao.daoImpl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.github.dreamyoung.mprelation.AutoMapper;
import com.wave.backend.dao.CartDao;
import com.wave.backend.dao.CartItemDao;
import com.wave.backend.mapper.CartItemMapper;
import com.wave.backend.entity.Cart;
import com.wave.backend.entity.CartItem;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.List;


@Component
public class CartItemDaoImpl implements CartItemDao {

    @Resource
    private CartItemMapper cartItemMapper;

    @Resource
    private CartDao cartDao;

    @Resource
    private AutoMapper autoMapper;
    @Override
    public void deleteOne(CartItem cartItem) {
        cartItemMapper.deleteById(cartItem);
    }

    @Override
    public CartItem findByBookIdAndUserId(Integer bookId, Integer userId) {

        Cart cart = cartDao.findByUserId(userId);
        QueryWrapper<CartItem> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("cartId", cart.getId());
        queryWrapper.eq("bookId", bookId);
        return cartItemMapper.selectOne(queryWrapper);
    }

    @Override
    public Integer saveOne(CartItem cartItem) {
        if (cartItem.getId() == null) {
            cartItemMapper.insert(cartItem);
        }
        return cartItemMapper.updateById(cartItem);
    }

    @Override
    public CartItem getVo(CartItem cartItem) {
        return autoMapper.mapperEntity(cartItem);
    }

    @Override
    public List<CartItem> getVos(List<CartItem> cartItems) {
        for (CartItem cartItem : cartItems) {
            autoMapper.mapperEntity(cartItem);
        }
        return cartItems;
    }


}
