// Khai báo API key và URL của ChatGPT
const API_KEY = "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";
const WHISPER_URL = "https://api.openai.com/v1/audio/transcriptions";
const API_URL = "https://api.openai.com/v1/completions";

document.getElementById('reply').addEventListener("click", async (e) => {
  e.preventDefault();
  var req = document.getElementById('msg_send').value;
  if (req == undefined || req == "") {
    return;
  } else {
    var res = "";
    await fetch(API_URL,
      {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${API_KEY}`
        },
        body: JSON.stringify({
          prompt: `${req}`,
          max_tokens: 256,
          temperature: 0,
          model: "text-davinci-003",
        })
      })
      .then(response => response.json())
      .then(data => {
        res = data.choices[0].text;
      })
      .catch(error => {
        console.error(error);
      })

    displayMessage(req, res);
    function scroll() {
      var scrollMsg = document.getElementById('msg')
      scrollMsg.scrollTop = scrollMsg.scrollHeight;
    }
    scroll();
  }
});



let startButton = document.getElementById("start-button");
let stopButton = document.getElementById("end-button");

async function speechToText(audio) {
  let req_msg = "";
  await fetch(WHISPER_URL,
    {
      method: 'POST',
      headers: {
        'Authorization': `Bearer ${API_KEY}`
      },
      body: audio
    })
    .then((response) => response.json())
    .then((data) => req_msg = data.text)
  console.log(req_msg);
  return req_msg;
}

async function getResponse(req_send) {
  let res_msg = "";
  await fetch(API_URL,
    {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${API_KEY}`
      },
      body: JSON.stringify({
        prompt: `${req_send}`,
        max_tokens: 256,
        temperature: 0,
        model: "text-davinci-003",
      })
    })
    .then(response => response.json())
    .then(data => {
      res_msg = data.choices[0].text;
    })
    .catch(error => {
      console.error(error);
    })
  return res_msg;
}

function TextToSpeech(res_msg) {
  const options = {
    method: 'POST',
    headers: {
      'content-type': 'application/json',
      'X-RapidAPI-Key': '9d4ccf4640msh81eea247cf018b1p1dff53jsn8f7b5776e4ba',
      'X-RapidAPI-Host': 'large-text-to-speech.p.rapidapi.com'
    },
    body: `{"text":"${res_msg}"}`
  };

  fetch('https://large-text-to-speech.p.rapidapi.com/tts', options)
    .then(response => response.blob())
    .then(blob => {
      // Create an audio element and set its source to the response data
      const audio = new Audio();
      audio.src = URL.createObjectURL(blob);

      // Play the audio
      audio.play();
    })
    .catch(err => console.error(err));
}


stopButton.disabled = false;

// Get access to the user's microphone
navigator.mediaDevices.getUserMedia({ audio: true })
  .then(function (stream) {
    // Create a MediaRecorder object to record the audio
    let chunks = [];
    let mediaRecorder = new MediaRecorder(stream);

    // When the recording is stopped, save the recorded audio as an MP3 file
    mediaRecorder.addEventListener("stop", async function () {
      let blob = new Blob(chunks, { type: "audio/wav" });
      
      // blob -> audio 
     
      let req_send = await speechToText(blob);
      // let res_msg = await getResponse(req_send);
      //getTextToSpeech(res_msg);
    });

    // Start recording when the user clicks a button
    startButton.addEventListener("click", function () {
      startButton.disabled = true;
      stopButton.disabled = false;
      mediaRecorder.start();
    });

    // Stop recording when the user clicks another button
    stopButton.addEventListener("click", function () {
      stopButton.disabled = true;
      startButton.disabled = false;
      mediaRecorder.stop();
    });

    // Collect the recorded audio data as it is captured
    mediaRecorder.addEventListener("dataavailable", function (event) {
      chunks.push(event.data);
    });
  })
  .catch(function (error) {
    console.log("Error:", error);
  });
















function displayMessage(req_msg, res_msg) {
  let data_req = document.createElement('div');
  let data_res = document.createElement('div');
  let container1 = document.createElement('div');
  let container2 = document.createElement('div');
  container1.setAttribute("class", "msgCon1");
  container2.setAttribute("class", "msgCon2");
  data_req.innerHTML = req_msg;
  data_res.innerHTML = res_msg;
  data_req.setAttribute("class", "right");
  data_res.setAttribute("class", "left");
  let message = document.getElementById('msg');
  message.appendChild(container1);
  message.appendChild(container2);
  container1.appendChild(data_req);
  container2.appendChild(data_res);
  document.getElementById('msg_send').value = "";
}


function init() {
  let res_elm = document.createElement("div");
  res_elm.innerHTML = "Hello, how can I help you?";
  res_elm.setAttribute("class", "left");
  document.getElementById('msg').appendChild(res_elm);
}
