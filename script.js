// Khai báo API key và URL của ChatGPT
const API_KEY = "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";
const API_URL = "https://api.openai.com/v1/completions";

function init() {
  let res_elm = document.createElement("div");
  res_elm.innerHTML = "Hello, how can I help you?";
  res_elm.setAttribute("class", "left");
  document.getElementById('msg').appendChild(res_elm);
}

document.getElementById('reply').addEventListener("click", async (e) => {
  e.preventDefault();

  var req = document.getElementById('msg_send').value;
  if (req == undefined || req == "") {
    return;
  } else {
    var res = "";
    try {
      const response = await axios({
        method: "POST",
        url: API_URL,
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${API_KEY}`,
        },
        data: {
          "model": "text-davinci-003",
          "prompt": `${req}`,
          "max_tokens": 256,
          "temperature": 0,
        },
      });
      res = response.data.choices[0].text.trim();
    } catch (error) {
      console.error(error);
    }


    let data_req = document.createElement('div');
    let data_res = document.createElement('div');

    let container1 = document.createElement('div');
    let container2 = document.createElement('div');

    container1.setAttribute("class", "msgCon1");
    container2.setAttribute("class", "msgCon2");

    data_req.innerHTML = req;
    data_res.innerHTML = res;

    data_req.setAttribute("class", "right");
    data_res.setAttribute("class", "left");

    let message = document.getElementById('msg');

    message.appendChild(container1);
    message.appendChild(container2);

    container1.appendChild(data_req);
    container2.appendChild(data_res);

    document.getElementById('msg_send').value = "";

    function scroll() {
      var scrollMsg = document.getElementById('msg')
      scrollMsg.scrollTop = scrollMsg.scrollHeight;
    }
    scroll();
  }
});