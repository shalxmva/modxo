
function downloadFile(bytestream, filename){
	var blob;
  
  try{
		blob= new Blob([bytestream], { type: "uf2" });
	}catch(e){
		//old browser, use BlobBuilder
		window.BlobBuilder=window.BlobBuilder || window.WebKitBlobBuilder || window.MozBlobBuilder || window.MSBlobBuilder;
		if(e.name==='InvalidStateError' && window.BlobBuilder){
			var bb=new BlobBuilder();
      bb.append(bytestream);
			blob=bb.getBlob("uf2");
		}else{
			throw new Error('Incompatible Browser');
			return false;
		}
	}
	saveAs(blob, filename);
}


class Block{
  constructor(address, data){
    this.address = address;
    this.block_size = 256;
    this.bytes = data;
  }

  encode(blockno, numblocks){
    var familyId = 0xe48bff56;
    var flags = 0x2000;       //withFamilyId flag
    var little_endian = true;
    var array= new ArrayBuffer(512);
    var view = new DataView(array);
    view.setUint32( 0,0x0A324655, little_endian); // UF2_MAGIC_START_0
    view.setUint32( 4,0x9E5D5157, little_endian);//UF2_MAGIC_START1
    view.setUint32(508,0x0AB16F30, little_endian);//UF2_MAGIC_END

    view.setUint32( 8,flags, little_endian);
    view.setUint32(12,this.address, little_endian);
    view.setUint32(16,this.block_size, little_endian);
    view.setUint32(20,blockno, little_endian);
    view.setUint32(24,numblocks, little_endian);
    view.setUint32(28,familyId, little_endian);
    var blockdata = new Uint8Array(view.buffer);
    blockdata.set(this.bytes,32);
    return blockdata;
  }
}

function get_flash_address_mask(size){
    maskbits = 0;
    size -= 1;

    while (size > 0){
        size >>= 1;
        maskbits+=1;
    }
      
    mem_capacity = 1<<(maskbits);
    address_mask = mem_capacity - 1;
    return address_mask;
}

function get_uf2_romaddr_mask_block( mask_address, romaddr_mask){
    var blocks = []
    

    address = mask_address;
    var data = new Uint8Array(256);
    var view = new DataView(data.buffer);
    view.setUint32( 0, romaddr_mask, true);
    //Copy 16 times a 256 block to fill a 4096 byte sector
    for(i =0;i<16;i++) {
        current_block = new Block(address, data);
        blocks.push(current_block);
        address+= current_block.block_size;
    }

    return blocks;
}

function get_uf2_blocks_from_file(filedata, flashrom_address){
    var blocks =[];
    var block_address = flashrom_address;
    var data_size = filedata.byteLength;
    var block_size = 256;
    for(i=0;i<data_size;i+=block_size){
      console.log("Block no: "+ i/block_size);
      current_block = new Block(block_address,new Uint8Array(filedata.slice(i,i+block_size)));
      blocks.push(current_block);
      block_address += block_size;
    }
    console.log("Total blocks: "+blocks.length);
    return blocks;
}

function write_uf2_file(data, outputfilename, flashrom_address, mask_address){

    var romaddr_mask = get_flash_address_mask(data.byteLength)
    var headerblocks = get_uf2_romaddr_mask_block(mask_address,romaddr_mask);
    var datablocks = get_uf2_blocks_from_file(data, flashrom_address);
    
    uf2blocks = headerblocks.concat(datablocks);

    console.log("Header blocks written: "+headerblocks.length);
    console.log("Data blocks written: "+datablocks.length);
    console.log("Total blocks written: "+uf2blocks.length);

    bytestream= new Uint8Array(uf2blocks.length*512);
    for(i =0;i<uf2blocks.length;i++){
      blockdata = uf2blocks[i].encode(i,uf2blocks.length);
      bytestream.set(blockdata,i*512);
    }

    downloadFile(bytestream , outputfilename);
}


function packBios(inputBuffer,filename) {
  write_uf2_file(inputBuffer, filename+".uf2", 0x10040000, 0x1003F000);
}

function dropHandler(ev) {
    console.log("File(s) dropped");
  
    // Prevent default behavior (Prevent file from being opened)
    ev.preventDefault();
  
    if (ev.dataTransfer.items) {
      // Use DataTransferItemList interface to access the file(s)
      [...ev.dataTransfer.items].forEach((item, i) => {
        // If dropped items aren't files, reject them
        if (item.kind === "file") {
          const file = item.getAsFile();
          console.log(`… file[${i}].name = ${file.name}`);

          console.log("Packing BIOS");
          console.log("File size is " + file.size);
          file.arrayBuffer().then(buffer => {packBios(buffer,file.name);})

        }
      });
    } else {
      // Use DataTransfer interface to access the file(s)
      [...ev.dataTransfer.files].forEach((file, i) => {
        console.log(`… file[${i}].name = ${file.name}`);
        file.arrayBuffer().then(buffer=>{packBios(buffer,file.name);});
      });
    }
  }

  function dragOverHandler(ev) {
    console.log("File(s) in drop zone");
  
    // Prevent default behavior (Prevent file from being opened)
    ev.preventDefault();
  }


  /* FileSaver.js (source: http://purl.eligrey.com/github/FileSaver.js/blob/master/src/FileSaver.js)
 * A saveAs() FileSaver implementation.
 * 1.3.8
 * 2018-03-22 14:03:47
 *
 * By Eli Grey, https://eligrey.com
 * License: MIT
 *   See https://github.com/eligrey/FileSaver.js/blob/master/LICENSE.md
 */
var saveAs = saveAs || function (c) { 
  "use strict";
   if (!(void 0 === c || "undefined" != typeof navigator && /MSIE [1-9]\./.test(navigator.userAgent))) { 
    var t = c.document, f = function () {  return c.URL || c.webkitURL || c },
       s = t.createElementNS("http://www.w3.org/1999/xhtml", "a"), 
       d = "download" in s, 
       u = /constructor/i.test(c.HTMLElement) 
       || 
       c.safari, 
       l = /CriOS\/[\d]+/.test(navigator.userAgent), 
       p = c.setImmediate || c.setTimeout, 
       v = function (t) { p(function () { throw t }, 0) }, 
       w = function (t) { setTimeout(function () { "string" == typeof t ? f().revokeObjectURL(t) : t.remove() }, 4e4) }, 
       m = function (t) { return /^\s*(?:text\/\S*|application\/xml|\S*\/\S*\+xml)\s*;.*charset\s*=\s*utf-8/i.test(t.type) ? new Blob([String.fromCharCode(65279), t], { type: t.type }) : t },
       r = function (t, n, e) { e || (t = m(t)); var r, o = this, a = "application/octet-stream" === t.type, i = function () { !function (t, e, n) { for (var r = (e = [].concat(e)).length; r--;) { var o = t["on" + e[r]]; if ("function" == typeof o) try { o.call(t, n || t) } catch (t) { v(t) } } }(o, "writestart progress write writeend".split(" ")) }; if (o.readyState = o.INIT, d) return r = f().createObjectURL(t), void p(function () { var t, e; s.href = r, s.download = n, t = s, e = new MouseEvent("click"), t.dispatchEvent(e), i(), w(r), o.readyState = o.DONE }, 0); !function () { if ((l || a && u) && c.FileReader) { var e = new FileReader; return e.onloadend = function () { var t = l ? e.result : e.result.replace(/^data:[^;]*;/, "data:attachment/file;"); c.open(t, "_blank") || (c.location.href = t), t = void 0, o.readyState = o.DONE, i() }, e.readAsDataURL(t), o.readyState = o.INIT } r || (r = f().createObjectURL(t)), a ? c.location.href = r : c.open(r, "_blank") || (c.location.href = r); o.readyState = o.DONE, i(), w(r) }() }, e = r.prototype; return "undefined" != typeof navigator && navigator.msSaveOrOpenBlob ? function (t, e, n) { return e = e || t.name || "download", n || (t = m(t)), navigator.msSaveOrOpenBlob(t, e) } : (e.abort = function () { }, e.readyState = e.INIT = 0, e.WRITING = 1, e.DONE = 2, e.error = e.onwritestart = e.onprogress = e.onwrite = e.onabort = e.onerror = e.onwriteend = null, function (t, e, n) { return new r(t, e || t.name || "download", n) }) } }("undefined" != typeof self && self || "undefined" != typeof window && window || this);