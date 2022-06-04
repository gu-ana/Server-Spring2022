import React, { useState, useEffect, useRef } from 'react';
import {
  Heading,
  Button,
  FormControl,
  FormLabel,
  FormErrorMessage,
  FormHelperText,
  Alert,
  AlertIcon,
  AlertTitle,
  AlertDescription,
} from '@chakra-ui/react';

import { BASE_URL, API_URI } from '../utils/url';

const CREATE_URL = BASE_URL + API_URI;

export var render_json = false;

export function createElementWithClass(tagName, classes) {
  const el = document.createElement(tagName);
  el.setAttribute("class", classes);

  return el;
}

export function createItem(color, desc) {
  const elmItem = createElementWithClass("div", "palette_item");
  const elmColor = createElementWithClass("div", "palette_color");
  const elmDesc= createElementWithClass("div", "palette_desc");
  const elmInput = createElementWithClass("div", "palette_input");

  elmColor.style.backgroundColor = color;
  elmInput.textContent = color.toUpperCase() ;
  elmDesc.textContent = desc;
  elmItem.appendChild(elmColor);
  elmItem.appendChild(elmInput);
  elmColor.appendChild(elmDesc);

  elmInput.onfocus = () => elmInput.select();

  return elmItem;
}


export const ShowPalette = ({palette}) => {
  useEffect( () => {
    if(render_json) {
      let json = JSON.parse(palette);
      document.querySelectorAll(".palette_item").forEach(e => e.remove());
      const el = document.querySelector(".palette");
      console.log(el);
      json["Palette"]["colors"].map(values => {
        el.appendChild(createItem(values.hex, values.name));
      });
    }
  });
}

const CreateHeader = () => {
  const [input_text, setInputText] = useState("");
  const [generated_palettes, updateArray] = useState([]);
  const isEmpty = input_text === "";
  const [data, setData] = useState([]);
  async function create_palette() {
    if(isEmpty) {
      return;
    }

    const response = await fetch(CREATE_URL, {
      method: 'POST',
      body: input_text
    })
      .then(response => {
        if (response.ok) {
          return response.json();
        }
      })
      .then(post => {
        setData(JSON.stringify(post));
        updateArray( arr => [...arr, post]);
        render_json = true;
      });

  }
  return (
    <div className="create">
      <h1> Palette Creation</h1>
      <FormControl isInvalid={isEmpty}>
        <FormLabel>
          Type a phrase/word for a curated color palette:
            <input type="text" placeholder="earth, water, air, fire, etc." onChange={(e) => setInputText(e.target.value)} />
              {
                !isEmpty ? (
                  <FormHelperText> 
                    Input text goes here.
                  </FormHelperText>
                ) : (
                  <FormErrorMessage>
                    Input text is required.
                  </FormErrorMessage>
                )
              }
        </FormLabel>
      </FormControl>
      <Button onClick={() => { create_palette()} }>Generate</Button>
      <ShowPalette palette={data} />
    </div>
  ); 
};


const CreateContainer = () => {
  return (
    <div>
        <div className="header">
          <CreateHeader />
        </div>
        <div className="palette">

        </div>
    </div>
  );
};


export default CreateContainer;