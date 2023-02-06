window.onload = function () {
  const div = document.createElement("div");
  div.innerHTML = "This is div";
  document.body.appendChild(div);

  const submit = document.querySelector("#button");

  // 发送POST请求
  submit.addEventListener("click", function () {
    let formData = new FormData();
    formData.append("name", "zs");
    formData.append("age", "40");
    formData.append("name", "ls");




    const xhr = new XMLHttpRequest();
    xhr.addEventListener("load", function () {
      console.log(xhr.responseText);
    });

    xhr.open("POST", "http://127.0.0.1:8080/query");
    // xhr.setRequestHeader("Content-Type", "text/xml");
    
    xhr.send(formData);
    
  });


  const fileinput = document.querySelector("#file");
  const uploadButton = document.querySelector("#uploadfile");

  // 上传文件
  uploadButton.onclick = function () {
    const file = fileinput.files[0];
    if (!file) return alert("请选择一个文件");

    let formData = new FormData();
    formData.append("file", file);




    const xhr = new XMLHttpRequest();
    xhr.addEventListener("load", function () {
      console.log(xhr.responseText);
    });

    xhr.open("POST", "http://127.0.0.1:8080/query");
    // xhr.setRequestHeader("Content-Type", "text/xml");
    
    xhr.send(formData);

    console.log(file);
  }

  const sendjson = document.querySelector("#sendjson");
  sendjson.onclick = function () {

    const xhr = new XMLHttpRequest();
    xhr.addEventListener("load", function () {
      console.log(xhr.response);
    });

    

    xhr.open("POST", "http://127.0.0.1:8080/query");

    xhr.setRequestHeader("Content-Type", "application/json");

    xhr.send(JSON.stringify({ name: "zs", age: 10 }));
  }

}