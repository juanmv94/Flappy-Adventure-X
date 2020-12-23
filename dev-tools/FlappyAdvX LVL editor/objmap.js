objmap=[
{name:"Alert",props:[{name:"alerted",type:0,mod:false},{name:"msg",type:3,mod:true}],rect:[[-4,-16],[8,16]],def:"\x00Hello World\x00".split("").map(x=>x.charCodeAt(0))},
{name:"Checkpoint",props:[],rect:[[-2,-16],[4,16]],def:[]},
{name:"Platform",props:[{name:"width",type:0,mod:true},{name:"amplitude y (div)",type:0,mod:true},{name:"speed y (^2)",type:0,mod:true},{name:"skew y (0-255)",type:0,mod:true},{name:"amplitude x (div)",type:0,mod:true},{name:"speed x (^2)",type:0,mod:true},{name:"skew x (0-255)",type:0,mod:true}],rect:[[-8,-4],[16,8]],def:[16,0,0,0,0,0,0]},
{name:"Pipe",props:[{name:"inverted",type:0,mod:true}],rect:[[0,-32],[16,32]],def:[0]},
{name:"Button",props:[{name:"pressed",type:0,mod:false},{name:"Flappy on it",type:0,mod:false}],rect:[[0,-4],[16,4]],def:[0,0]},
{name:"Door",props:[{name:"open",type:0,mod:true}],rect:[[-3,-16],[6,16]],def:[0]},
{name:"Block",props:[],rect:[[0,-16],[16,16]],def:[]}
];