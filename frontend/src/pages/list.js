


import { useForceUpdate,  palettes, createItem, render_json } from './create';
import React, { useState, useEffect, useRef } from 'react';
import {Button} from '@chakra-ui/react';



const ListContainer = () => {
    const forceUpdate = useForceUpdate();
    console.log(palettes);
    console.log("hi")
    return (
        <div className="List">
            <h1> Previous palettes generated are listed here: </h1>
            <Button onClick={ forceUpdate }>List!</Button>
            <div className="listPalette">

            </div>
            <ShowPalettes palettes={palettes} />
        </div>
    )
}

const ShowPalettes = ({palettes}) => {
    function palette_json(palette) {
        const el = document.querySelector(".listPalette");
        palette["Palette"]["colors"].map(values => {
          el.appendChild(createItem(values.hex, values.name));
        });
    }
    useEffect( () => {
      if(render_json) {
          palettes.forEach(palette_json);
       
      }
    });
  }

export default ListContainer;