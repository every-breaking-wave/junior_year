package com.wave.backend.dao.daoImpl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.wave.backend.dao.BookDao;
import com.wave.backend.fulltextsearching.FilesPositionConfig;
import com.wave.backend.fulltextsearching.ReadWriteFiles;
import com.wave.backend.fulltextsearching.SearchFiles;
import com.wave.backend.mapper.BookMapper;
import com.wave.backend.entity.Book;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.List;

@Component
public class BookDaoImpl implements BookDao {
    @Resource
    private BookMapper bookMapper;

    @Override
    public Integer saveOne(Book book) {
        if (ifBookExist(book.getBookName())) {
            ReadWriteFiles.create_docs_files(book.getId(), book.getBookDescription(), book.getAuthor(), book.getBookName(), true);
            return bookMapper.updateById(book);
        }
        int newBookId = bookMapper.insert(book);
        if (newBookId > 0) {
            ReadWriteFiles.create_docs_files(newBookId, book.getBookDescription(),  book.getAuthor(),book.getBookName(), true);
        }
        return newBookId;
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
        if (!key.equals("default")) {
            queryWrapper.like("bookName", key);
        }
        return bookMapper.selectList(queryWrapper);
    }

    @Override
    public Book findById(Integer id) {
        return bookMapper.selectById(id);
    }

    @Override
    public Boolean deleteById(Integer id) {
        return bookMapper.deleteById(id) > 0;
    }

    @Override
    public List<Book> fulltextBookSearching(String text) {
        List<Book> bookList = new ArrayList<>();
        try {
            String[] args = {"-index", FilesPositionConfig.indexPath, "-query", text};
            List<Integer> bookidList = SearchFiles.search_interface(args);
            for (Integer integer : bookidList) {
                Book book = findById(integer);
                /* 如果该书没有被删除 */
                if (book != null && book.getIsDeleted() == 0) bookList.add(book);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return bookList;
    }

    @Override
    public List<Book> getAllBooks() {
        return bookMapper.selectList(new QueryWrapper<>());
    }


}
