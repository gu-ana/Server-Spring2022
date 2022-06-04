


import {  createItem, render_json } from './create';
import React, { useState, useEffect, useRef } from 'react';


const ListContainer = () => {
    return (
        <div className="List">
            <h1> Previous words used are listed here: </h1>
            {/* <ShowPalettes palettes={generated_palettes} /> */}
        </div>
    )
}

// export const ShowPalettes = ({palettes}) => {
//     function palette_json(palette) {
//         let json = JSON.parse(palette);
//         const el = document.querySelector(".palette");
//         console.log(el);
//         json["Palette"]["colors"].map(values => {
//           el.appendChild(createItem(values.hex, values.name));
//         });
//     }
//     useEffect( () => {
//       if(render_json) {
//           palettes.forEach(palette_json());
       
//       }
//     });
//   }

export default ListContainer;