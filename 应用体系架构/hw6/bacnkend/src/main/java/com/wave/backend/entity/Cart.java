package com.wave.backend.entity;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableField;
import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import com.github.dreamyoung.mprelation.*;
import lombok.Data;

import java.io.Serializable;
import java.util.List;

@TableName(value ="carts")
@Data
public class Cart implements Serializable {

    @TableId(type = IdType.AUTO)
    private Integer id;

    private Integer userId;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;

    //一对多
    @TableField(exist = false)
    @OneToMany(cascade = CascadeType.ALL, fetch = FetchType.EAGER) //一对多默认为延迟加载,即@Lazy/@Lazy(true)/或此时不标注
    @JoinColumn(name="id",referencedColumnName = "cartId")//@TableId与一方相关属性中@TableField名称保持一致时@JoinColumn可省略
    private List<CartItem> cartItems;

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
        Cart other = (Cart) that;
        return (this.getId() == null ? other.getId() == null : this.getId().equals(other.getId()))
            && (this.getUserId() == null ? other.getUserId() == null : this.getUserId().equals(other.getUserId()));
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((getId() == null) ? 0 : getId().hashCode());
        result = prime * result + ((getUserId() == null) ? 0 : getUserId().hashCode());
        return result;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(getClass().getSimpleName());
        sb.append(" [");
        sb.append("Hash = ").append(hashCode());
        sb.append(", id=").append(id);
        sb.append(", userId=").append(userId);
        sb.append(", serialVersionUID=").append(serialVersionUID);
        sb.append("]");
        return sb.toString();
    }
}