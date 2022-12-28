package com.wave.backend.dao.daoImpl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.wave.backend.dao.BookDao;
import com.wave.backend.mapper.BookMapper;
import com.wave.backend.entity.Book;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.List;

@Component
public class BookDaoImpl implements BookDao {
    @Resource
    private BookMapper bookMapper;
    @Override
    public Integer saveOne(Book book) {
        if(ifBookExist(book.getBookName())){
            return bookMapper.updateById(book);
        }
        else return bookMapper.insert(book);
    }

    @Override
    public Boolean ifBookExist(String bookName) {
        QueryWrapper<Book> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("bookName", bookName);
        return bookMapper.selectCount(queryWrapper) != 0;
    }

    @Override
    public List<Book> findBySearchKey(String key) {
        QueryWrapper<Book> queryWrapper = new QueryWrapper<>();
        // 过滤已删除书本
        queryWrapper.eq("isDeleted", 0);
        // 若为空查询，返回所有书本
        if(!key.equals("default")){
            queryWrapper.like("bookName",key);
        }
        return bookMapper.selectList(queryWrapper);
    }

    @Override
    public Book findById(Integer id) {
        return bookMapper.selectById(id);
    }
}
