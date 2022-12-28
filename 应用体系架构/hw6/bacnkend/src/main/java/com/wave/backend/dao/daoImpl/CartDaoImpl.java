package com.wave.backend.dao.daoImpl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.github.dreamyoung.mprelation.AutoMapper;
import com.wave.backend.dao.CartDao;
import com.wave.backend.mapper.CartMapper;
import com.wave.backend.entity.Cart;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;


@Component
public class CartDaoImpl implements CartDao {

    @Resource
    private CartMapper cartMapper;

    @Resource
    private AutoMapper autoMapper;


    @Override
    public Integer saveOne(Cart cart) {
        if (cart.getId() == null) {
            cartMapper.insert(cart);
        }
        return cartMapper.updateById(cart);
    }

    @Override
    public Cart findByUserId(Integer userId) {
        QueryWrapper<Cart> queryWrapper1 = new QueryWrapper<>();
        queryWrapper1.eq("userId",userId);
        return cartMapper.selectOne(queryWrapper1);
    }

    @Override
    public Cart getVo(Cart cart) {
        return autoMapper.mapperEntity(cart);
    }
}
