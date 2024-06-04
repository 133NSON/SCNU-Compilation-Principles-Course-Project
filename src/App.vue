<template>
  <div>
    <div class="pad-div">
      <img id="ppp" class="round-image" src="../Zafkiel.png">
      <div>
        <p class="text-container">Zafkiel</p>
        <p class="text-container_">Copyright © 2024 Benson</p>
      </div>
    </div>
    <div>
      <textarea ref="regInput" id="regInput" rows="10" cols="50" placeholder="regular expression" style="margin-right: 10px;"></textarea>
      <textarea ref="srcInput" id="srcInput" rows="10" cols="50" placeholder="test code" style="margin-right: 10px;"></textarea>
      <textarea ref="rulesInput" id="rulesInput" rows="10" cols="50" placeholder="BNF rules" style="margin-right: 10px;"></textarea>
    </div>
    <div style="width: 1800px; margin-top: 10px;">
      <span class="span-container">Task1</span>
      <label class="custom-file-upload">
        <input type="file" id="file1" @change="handleFile1Upload" />
        Browse...
      </label>
      <button class="button-container" @click="save_reg">Save Reg</button>
      <button class="button-container" @click="save_src">Save Src</button>
      <button class="button-container" @click="lab1">Lab1</button>
      <button class="button-container" @click="nfa">NFA</button>
      <button class="button-container" @click="dfa">DFA</button>
      <button class="button-container" @click="mindfa">Min DFA</button>
      <button class="button-container" @click="gen_code">Gen Code</button>
      <button class="button-container" @click="test_code">Test Code</button>
      <button class="button-container" @click="clear1">Clear</button>
    </div>
    <div class="file-msg">
      <p v-if="selectedFile1Name">Selected File: {{ selectedFile1Name }}</p>
      <p v-else>No file selected.</p>
    </div>
    <div style="width: 1800px; margin-top: 10px;">
      <span class="span-container">Task2</span>
      <label class="custom-file-upload">
        <input type="file" id="file2" @change="handleFile2Upload" />
        Browse...
      </label>
      <button class="button-container" @click="save_rules">Save Rules</button>
      <button class="button-container" @click="lab2">Lab2</button>
      <button class="button-container" @click="ff">FIRST/FOLLOW</button>
      <button class="button-container" @click="lr">LR(0) DFA</button>
      <button class="button-container" @click="slr">SLR(1)</button>
      <button class="button-container" @click="analyse_code">Analyse Code</button>
      <button class="button-container" @click="gen_tree">Gen Tree</button>
      <button class="button-container" @click="clear2">Clear</button>
    </div>
    <div class="file-msg">
      <p v-if="selectedFile1Name">Selected File: {{ selectedFile2Name }}</p>
      <p v-else>No file selected.</p>
    </div>
    <div class="code_container">
      <div class="code_item">
        <pre v-if="sourceCode">{{ sourceCode }}</pre>
      </div>
      <div id="tableContainer"></div>
      <pre id="codeResult" style="color: yellow;"></pre>
    </div>
  </div>
</template>

<script>
import axios from 'axios';
import * as d3 from 'd3';


export default {
  name: 'App',
  data() {
    return {
      // task1
      file1: null,
      selectedFile1Name: '',
      sourceCode:  '',
      nfaResult: '',
      dfaResult: '',
      mindfaResult: '',
      codeResult: '',
      // task2
      file2: null,
      selectedFile2Name: '',
      ffResult: '',
      lrResult: '',
      slrResult: '',
      analyseResult: '',
      margin: { top: 20, right: 20, bottom: 20, left: 20 },
      treeData: null
    };
  },
  methods: {
    // task1
    handleFile1Upload(event) {
      this.file1 = event.target.files[0];
      this.selectedFile1Name = this.file1.name;
      const reader = new FileReader();
      reader.onload = () => {
        this.sourceCode = reader.result;
        this.$refs.regInput.value = this.sourceCode; // Set the value of the textarea
      };
      reader.readAsText(this.file1, 'UTF-8');
    },
    save_reg() {
      const formData = new FormData();

      var textInput = document.getElementById("regInput").value;
      this.sourceCode = textInput;
      formData.append('reg', textInput);

      axios.post('http://localhost:8000/upload_reg', formData)
        .then(response => {
          console.log(response.data);
        })
        .catch(error => {
          console.error(error);
        });
    },
    save_src() {
      const formData = new FormData();

      var textInput = document.getElementById("srcInput").value;
      formData.append('src', textInput);

      axios.post('http://localhost:8000/upload_src', formData)
        .then(response => {
          console.log(response.data);
        })
        .catch(error => {
          console.error(error);
        });
    },
    lab1() {
      axios.post('http://localhost:8000/lab1')
      .then(response => {
          console.log(response.data);
        })
        .catch(error => {
          console.error(error);
        });
    },
    nfa() {
      axios.post('http://localhost:8000/nfa')
        .then(response => {
          console.log(response.data);
          this.nfaResult = response.data;
          this.$nextTick(() => {
            document.getElementById("codeResult").innerHTML = "";
            document.getElementById("tableContainer").innerHTML = this.nfaResult;
          });
        })
        .catch(error => {
          console.error(error);
        });
    },
    dfa() {
      axios.post('http://localhost:8000/dfa')
        .then(response => {
          console.log(response.data);
          this.dfaResult = response.data;
          this.$nextTick(() => {
            document.getElementById("codeResult").innerHTML = "";
            document.getElementById("tableContainer").innerHTML = this.dfaResult;
          });
        })
        .catch(error => {
          console.error(error);
        });
    },
    mindfa() {
      axios.post('http://localhost:8000/min')
        .then(response => {
          console.log(response.data);
          this.mindfaResult = response.data;
          this.$nextTick(() => {
            document.getElementById("codeResult").innerHTML = "";
            document.getElementById("tableContainer").innerHTML = this.mindfaResult;
          });
        })
        .catch(error => {
          console.error(error);
        });
    },
    gen_code() {
      axios.post('http://localhost:8000/gen_code')
        .then(response => {
          console.log(response.data);
          this.codeResult = response.data;
          this.$nextTick(() => {
            document.getElementById("tableContainer").innerHTML = "";
            document.getElementById("codeResult").innerHTML = this.codeResult;
          });
        })
        .catch(error => {
          console.error(error);
        });
    },
    test_code() {
      axios.post('http://localhost:8000/test_code')
        .then(response => {
          console.log(response.data);
          this.codeResult = response.data;
          this.$nextTick(() => {
            document.getElementById("tableContainer").innerHTML = "";
            document.getElementById("codeResult").innerHTML = this.codeResult;
          });
        })
        .catch(error => {
          console.error(error);
        });
    },
    clear1() {
      this.file1 = null,
      this.selectedFile1Name = ''
      this.sourceCode = ''
      this.nfaResult = ''
      this.dfaResult = ''
      this.mindfaResult = ''
      this.codeResult = ''
      document.getElementById("tableContainer").innerHTML = ""
    },
    // task1 end
    // task2
    handleFile2Upload(event) {
      this.file2 = event.target.files[0];
      this.selectedFile2Name = this.file2.name;
      const reader = new FileReader();
      reader.onload = () => {
        this.sourceCode = reader.result;
        this.$refs.rulesInput.value = this.sourceCode; // Set the value of the textarea
      };
      reader.readAsText(this.file2, 'UTF-8');
    },
    save_rules() {
      const formData = new FormData();

      var textInput = document.getElementById("rulesInput").value;
      this.sourceCode = textInput;
      formData.append('rules', textInput);

      axios.post('http://localhost:8000/upload_bnf', formData)
        .then(response => {
          console.log(response.data);
        })
        .catch(error => {
          console.error(error);
        });
    },
    lab2() {
      axios.post('http://localhost:8000/lab2')
        .then(response => {
          console.log(response.data);
          this.$nextTick(() => {
            this.sourceCode = response.data;
          });
        })
        .catch(error => {
          console.error(error);
        });
        this.ruleStr = "New Rules:"
    },
    ff() {
      axios.post('http://localhost:8000/ff')
        .then(response => {
          console.log(response.data);
          this.ffResult = response.data;
          this.$nextTick(() => {
            document.getElementById("codeResult").innerHTML = "";
            document.getElementById("tableContainer").innerHTML = this.ffResult;
          });
        })
        .catch(error => {
          console.error(error);
        });
    },
    lr() {
      axios.post('http://localhost:8000/lr')
        .then(response => {
          console.log(response.data);
          this.lrResult = response.data;
          this.$nextTick(() => {
            document.getElementById("codeResult").innerHTML = "";
            document.getElementById("tableContainer").innerHTML = this.lrResult;
          });
        })
        .catch(error => {
          console.error(error);
        });
    },
    slr() {
      axios.post('http://localhost:8000/slr')
        .then(response => {
          console.log(response.data);
          this.slrResult = response.data;
          this.$nextTick(() => {
            document.getElementById("codeResult").innerHTML = "";
            document.getElementById("tableContainer").innerHTML = this.slrResult;
          });
        })
        .catch(error => {
          console.error(error);
        });
    },
    analyse_code() {
      axios.post('http://localhost:8000/analyse_code')
        .then(response => {
          console.log(response.data);
          this.analyseResult = response.data;
          this.$nextTick(() => {
            document.getElementById("codeResult").innerHTML = "";
            document.getElementById("tableContainer").innerHTML = this.analyseResult;
          });
        })
        .catch(error => {
          console.error(error);
        });
    },
    gen_tree() {
      axios.post('http://localhost:8000/gen_tree')
        .then(response => {
          console.log(response.data);

          document.getElementById("codeResult").innerHTML = ""
          this.treeData = response.data;
          this.drawTree();
        })
        .catch(error => {
          console.error(error);
        });
    },
    drawTree() {
      // Clear the container
      d3.select("#tableContainer").html("");

      // Create a hierarchy from the data
      let root = d3.hierarchy(this.treeData);

      // Create a tree layout
      let treeLayout = d3.tree().nodeSize([20, 60]);

      // Compute the tree layout
      treeLayout(root);

      // Append an SVG element to the container and enable zoom
      let svgContainer = d3.select("#tableContainer").append("svg")
        .attr("width", 1500 + this.margin.left + this.margin.right)
        .attr("height", 1200 + this.margin.top + this.margin.bottom)
        .call(d3.zoom().on("zoom", function (event) {
          svg.attr("transform", event.transform);
        }));

      let svg = svgContainer.append("g")
        .attr("transform", "translate(" + this.margin.left + "," + this.margin.top + ")");

      // Draw the links (branches)
      svg.selectAll(".link")
        .data(root.links())
        .enter().append("path")
        .attr("class", "link")
        .attr("d", d3.linkHorizontal()
          .x(function(d) { return d.y; }) // Swap x and y
          .y(function(d) { return d.x; })) // Swap x and y
        .attr("stroke", "yellow");

      // Draw the nodes
      let nodes = svg.selectAll(".node")
        .data(root.descendants())
        .enter().append("g")
        .attr("class", function(d) { return "node" + (d.children ? " node--internal" : " node--leaf"); })
        .attr("transform", function(d) { return "translate(" + d.y + "," + d.x + ")"; }); // Swap x and y

      nodes.append("circle")
        .attr("r", 10)
        .style("fill", "red");

      nodes.append("text")
        .attr("dy", 16) // Move the text 15 units downwards
        .attr("x", 0) // Center the text horizontally
        .style("text-anchor", "middle") // Center the text
        .style("fill", "white")
        .text(function(d) { return d.data.value; });
    },
    clear2() {
        this.file2 = null,
        this.selectedFile2Name = ''
        this.ffResult = ''
        this.lrResult = ''
        this.slrResult = ''
        this.analyseResult = ''
        this.treeData = null
        document.getElementById("tableContainer").innerHTML = ""
    },
    // task2 end
  }
}
</script>

<style>
.round-image {
  border-radius: 50%;
}

#ppp {
  position: relative;
  width: 150px;
  height: 150px;
}

#ppp > img {
  position: relative;
  width: 100%;
  height: 100%;
}

.pad-div {
  margin-bottom: 20px;
}

.button-container {
  background-color:transparent;
  border-style:none;
  border: 4px solid red;
  display: inline-block;
  padding: 10px 25px;
  cursor: pointer;
  margin-right: 20px;
  font-weight: bold;
  color: white;
  border-radius: 20px;
}

.span-container {
    background-color: red;
    color: white;
    padding: 6px 15px;
    margin-right: 20px;
    font-weight: bold;
}

input[type="file"] {
    display: none;
}

.custom-file-upload {
  border: 4px solid red;
  display: inline-block;
  padding: 6px 15px;
  cursor: pointer;
  margin-right: 20px;
  font-weight: bold;
  color: white;
  border-radius: 20px;
}

.text-container {
  font-weight: bold;
  font-size: 200%;
  color: grey;
}

.text-container_ {
  color: grey;
}

.code_container {
  margin-top: 10px;
  display: flex;
}

.code_item {
  color: yellow;
  margin-right: 100px;
  white-space: nowrap;
}

.file-msg {
  color: darkgray;
}

table {
  border-collapse: collapse;
}

td, th {
  color: yellow;
  border: 1px solid yellow;
  padding: 5px;
}

#regInput {
  background-color: transparent;
  border: solid yellow;
  color: yellow;
  margin-right: 5px;
}

#srcInput {
  background-color: transparent;
  border: solid yellow;
  color: yellow;
  margin-right: 5px;
}

#rulesInput {
  background-color: transparent;
  border: solid yellow;
  color: yellow;
}

svg {
  font: 10px sans-serif;
}
</style>