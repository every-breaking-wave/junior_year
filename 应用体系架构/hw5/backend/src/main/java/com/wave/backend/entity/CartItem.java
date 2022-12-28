package com.wave.backend.entity;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableField;
import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import java.io.Serializable;

import com.github.dreamyoung.mprelation.JoinColumn;
import com.github.dreamyoung.mprelation.Lazy;
import com.github.dreamyoung.mprelation.OneToOne;
import lombok.Data;

/**
 *
 * @TableName caritem
 */
@TableName(value ="cart_items")
@Data
public class CartItem implements Serializable {

    @TableId(type = IdType.AUTO)
    private Integer id;

    private Integer cartId;

    private Integer number;

    private Integer bookId;


    // TODO: 可以级联一本书
    //一对一
    @TableField(exist = false)
    @OneToOne //一对多默认为延迟加载,即@Lazy/@Lazy(true)/或此时不标注
    @JoinColumn(name="bookId",referencedColumnName = "id")//@TableId与一方相关属性中@TableField名称保持一致时@JoinColumn可省略
    @Lazy(false)
    private Book book;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;


    @Override
    public boolean equals(Object that) {
        if (this == that) {
            return true;
        }
        if (that == null) {
            return false;
        }
        if (getClass() != that.getClass()) {
            return false;
        }
        CartItem other = (CartItem) that;
        return (this.getId() == null ? other.getId() == null : this.getId().equals(other.getId()))
                && (this.getCartId() == null ? other.getCartId() == null : this.getCartId().equals(other.getCartId()))
                && (this.getNumber() == null ? other.getNumber() == null : this.getNumber().equals(other.getNumber()))
                && (this.getBookId() == null ? other.getBookId() == null : this.getBookId().equals(other.getBookId()));
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((getId() == null) ? 0 : getId().hashCode());
        result = prime * result + ((getCartId() == null) ? 0 : getCartId().hashCode());
        result = prime * result + ((getNumber() == null) ? 0 : getNumber().hashCode());
        result = prime * result + ((getBookId() == null) ? 0 : getBookId().hashCode());
        return result;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(getClass().getSimpleName());
        sb.append(" [");
        sb.append("Hash = ").append(hashCode());
        sb.append(", id=").append(id);
        sb.append(", cartId=").append(cartId);
        sb.append(", number=").append(number);
        sb.append(", bookId=").append(bookId);
        sb.append(", serialVersionUID=").append(serialVersionUID);
        sb.append(", Book=").append(book);
        sb.append("]");
        return sb.toString();
    }
}