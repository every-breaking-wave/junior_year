package com.wave.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.wave.backend.dao.CartDao;
import com.wave.backend.dao.CartItemDao;
import com.wave.backend.entity.CartItem;
import com.wave.backend.mapper.CartItemMapper;
import com.wave.backend.service.CartItemService;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.List;

/**
 * @author Feng
 * @description 针对表【caritem】的数据库操作Service实现
 * @createDate 2022-05-15 13:49:47
 */
@Service
public class CartItemServiceImpl extends ServiceImpl<CartItemMapper, CartItem>
        implements CartItemService {
    @Resource
    private CartItemDao cartItemDao;

    @Resource
    private CartDao cartDao;


    @Override
    public Boolean addByCount(Integer userId, Integer bookId, Integer count) {

        CartItem cartItem;
        if ((cartItem = cartItemDao.findByBookIdAndUserId(bookId, userId)) == null) {// 如果目前没有这个商品,新建carItem
            cartItem = new CartItem();
            cartItem.setBookId(bookId);
            cartItem.setCartId(cartDao.findByUserId(userId).getId());
            cartItem.setNumber(count);
        } else {// 否则更新数量
            cartItem.setNumber(cartItem.getNumber() + count);
        }
        cartItemDao.saveOne(cartItem);
        return true;
    }

    @Override
    public Boolean minByCount(Integer userId, Integer bookId, Integer count) {
        CartItem cartItem;
        if ((cartItem = cartItemDao.findByBookIdAndUserId(bookId, userId)) == null) {// 如果目前没有这个商品,减小失败
            return false;
        } else if (cartItem.getNumber() <= count) {
            return false;
        } else {
            cartItem.setNumber(cartItem.getNumber() - count);
        }
        cartItemDao.saveOne(cartItem);
        return true;
    }

    @Override
    public Boolean delOne(Integer userId, Integer bookId, Integer count) {
        CartItem cartItem;
        if ((cartItem = cartItemDao.findByBookIdAndUserId(bookId, userId)) == null) {// 如果目前没有这个商品,删除失败
            return false;
        }
        cartItemDao.deleteOne(cartItem);
        return true;
    }

    @Override
    public List<CartItem> getVos(List<CartItem> cartItems) {
        return cartItemDao.getVos(cartItems);
    }
}
