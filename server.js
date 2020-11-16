const express = require("express");
const bodyParser = require("body-parser");
const chck = require('bindings')('user_checker');

const app = express();
const urlencodedParser = bodyParser.urlencoded({ extended: false });

app.get("/", urlencodedParser, function (request, response) {
    response.sendFile(__dirname + "/request.html");
});

app.post("/", urlencodedParser, function (request, response) {
    if (!request.body) return response.sendStatus(400);
    if ('user_name' in request.body){
        let name = request.body['user_name'];
        let ans = chck.CheckUser(name);
        console.log(name,ans);
        response.send(ans?`Пользователь ${name} есть`:`Пользователя ${name} нет`);
    }
        
});

app.listen(1337);
