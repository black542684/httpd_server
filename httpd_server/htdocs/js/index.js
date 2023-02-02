window.onload = function () {
  const div = document.createElement("div");
  div.innerHTML = "This is div";
  document.body.appendChild(div);

  const submit = document.querySelector("#button");

  submit.addEventListener("click", function () {
    let formData = new FormData();
    formData.append("name", "zs");
    formData.append("age", "40");
    const xhr = new XMLHttpRequest();
    xhr.addEventListener("load", function () {
      console.log(xhr.responseText);
    });

    xhr.open("POST", "http://127.0.0.1:8080/query");
    xhr.setRequestHeader("Content-Type", "application/json");
    let obj = {
      name: "zs",
      age: 12
    };
    xhr.send(JSON.stringify(obj));
    
  });
}