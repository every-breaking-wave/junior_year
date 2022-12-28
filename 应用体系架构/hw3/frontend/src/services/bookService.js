
import axios from "axios";
import Pubsub from "pubsub-js";



export const getBook = (id, callback) => {

    axios.post(`/api/book/getbook/${id}`).then(
        response => {
            console.log("id ="+id)
            console.log("请求成功", response.data);
            if(response.data != null){
                // Pubsub.publish('BookDetail',{isLoading:false, bookList: response.data.bookList})
                const book  = response.data;
                callback(book);
            }
        },
        error => {
            Pubsub.publish('searchBook',{err: error.message})
        }
    )
};

export const getBooks = (id, callback) => {

    axios.post(`/api/book/getbooks/${id}`).then(
        response => {
            console.log("id ="+id)
            console.log("请求成功", response.data);
            if(response.data != null){
                const books  = response.data;
                console.log({callback})
                callback(books);
            }
        },
    )
};

export const getBooksById = (id, callback) => {

    axios.post(`/api/book/getbooks/${id}`).then(
        response => {
            console.log("id ="+id)
            console.log("请求成功", response.data);
            if(response.data != null){
                const book  = response.data.book;
                console.log({callback})
                callback(book);
            }
        },
        error => {
            Pubsub.publish('searchBook',{err: error.message})
        }
    )
};

 export async function  getAllBooks (){
     return await axios.post(`/api/book/search/default`)
         .then(response => {
            console.log("请求成功", response.data);
            if(response.data != null){
                const bookList  = response.data;
                return bookList
            }
        },
    )
}


export  function  refreshBook (newBook){
     console.log(newBook)
    fetch( "/api/admin/update-book", {
        method: "POST",
        headers: new Headers({
            "Content-Type": "application/json"
        }),
        body: JSON.stringify(newBook)
    }).catch(() => {})
}



export  function  delBook (book){
    console.log(book)
    fetch( "/api/admin/del-book", {
        method: "POST",
        headers: new Headers({
            "Content-Type": "application/json"
        }),
        body: JSON.stringify(book)
    }).catch(() => {})
}

