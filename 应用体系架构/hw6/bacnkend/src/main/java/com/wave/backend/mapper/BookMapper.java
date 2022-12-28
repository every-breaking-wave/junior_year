package com.wave.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.wave.backend.entity.Book;
import org.apache.ibatis.annotations.Mapper;

/**
* @author Feng
* @description 针对表【book】的数据库操作Mapper
* @createDate 2022-05-09 23:37:44
* @Entity generator.domain.Book
*/
@Mapper
public interface BookMapper extends BaseMapper<Book> {


}
