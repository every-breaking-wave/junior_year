package com.wave.backend.dao.daoImpl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.wave.backend.dao.BookDao;
import com.wave.backend.entity.BookInfo;
import com.wave.backend.entity.BookLabel;
import com.wave.backend.entity.LabelOfBook;
import com.wave.backend.fulltextsearching.FilesPositionConfig;
import com.wave.backend.fulltextsearching.ReadWriteFiles;
import com.wave.backend.fulltextsearching.SearchFiles;
import com.wave.backend.mapper.BookMapper;
import com.wave.backend.entity.Book;
import com.wave.backend.mapper.LabelOfBookMapper;
import com.wave.backend.repository.BookInfoRepository;
import com.wave.backend.repository.BookLabelRepository;
import javafx.scene.control.Label;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.*;

@Slf4j
@Component
public class BookDaoImpl implements BookDao {
    @Resource
    private BookMapper bookMapper;

    @Resource
    private BookInfoRepository bookInfoRepository;

    @Resource
    private BookLabelRepository bookLabelRepository;

    @Resource
    private LabelOfBookMapper labelOfBookMapper;

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
        List<Book> books = bookMapper.selectList(queryWrapper);
        for(Book book : books){
            BookInfo bookInfo = bookInfoRepository.findBookInfoByBookId(book.getId());
            if(bookInfo != null){
                book.setCover("data:image/jpeg;base64," + bookInfo.getIconBase64());
            } else {
                book.setCover(null);
            }
        }
        return books;
    }

    @Override
    public Book findById(Integer id) {
        Book book = bookMapper.selectById(id);
        BookInfo bookInfo = bookInfoRepository.findBookInfoByBookId(id);
        if(bookInfo != null){
            book.setCover("data:image/jpeg;base64," + bookInfo.getIconBase64());
            log.info("get image base64 from mongoDB");
        } else {
            book.setCover(null);
        }
        return book;
    }

    @Override
    public Boolean deleteById(Integer id) {

        Boolean flag = bookMapper.deleteById(id) > 0;
        if(flag){
            bookInfoRepository.deleteById(id);
        }
        return flag;
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
        for(Book book : bookList){
            BookInfo bookInfo = bookInfoRepository.findBookInfoByBookId(book.getId());
            if(bookInfo != null){
                book.setCover("data:image/jpeg;base64," + bookInfo.getIconBase64());
            } else {
                book.setCover(null);
            }
        }
        return bookList;
    }

    @Override
    public List<Book> getAllBooks() {
        return bookMapper.selectList(new QueryWrapper<>());
    }

    @Override
    public List<BookInfo> getBookInfo() {
        return bookInfoRepository.findAll();
    }

    @Override
    public List<Book> findByLabelName(String labelName) {

        QueryWrapper<Book> queryWrapper = new QueryWrapper<>();
        BookLabel bookLabel = bookLabelRepository.findBookLabelByLabelName(labelName);
        if(bookLabel == null){
            log.info("no such label name");
        }
        List<BookLabel> recBookLabels = bookLabelRepository.getLabelsInTwoSteps(labelName);
        assert bookLabel != null;
        recBookLabels.addAll(bookLabel.getRecoms());
        Set<Integer> labelIdSet = new HashSet<>();
        for (BookLabel recBookLabel : recBookLabels) {
            labelIdSet.add(recBookLabel.getLabelId());
        }
        QueryWrapper<LabelOfBook> queryWrapper1 = new QueryWrapper<>();
        queryWrapper1.in("labelId", labelIdSet);
        List<LabelOfBook> labelOfBookList = labelOfBookMapper.selectList(queryWrapper1);
        Set<Integer> bookIdList = new HashSet<>();
        for (LabelOfBook labelOfBook : labelOfBookList){
            bookIdList.add(labelOfBook.getBookId());
        }
        queryWrapper.in("id", bookIdList);
        List<Book> books = bookMapper.selectList(queryWrapper);
        for(Book book : books){
            BookInfo bookInfo = bookInfoRepository.findBookInfoByBookId(book.getId());
            if(bookInfo != null){
                book.setCover("data:image/jpeg;base64," + bookInfo.getIconBase64());
            } else {
                book.setCover(null);
            }
        }
        return books;

    }


}
