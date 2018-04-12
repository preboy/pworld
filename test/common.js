// 返回一个大于等于0，小于1的伪随机数
exports.getRandom = () => {
  return Math.random();
}


// 返回一个介于min和max之间的随机数
exports.getRandomArbitrary = (min, max) => {
  return Math.random() * (max - min) + min;
}


// 返回一个介于min和max之间的整型随机数
// Using Math.round() will give you a non-uniform distribution!
exports.getRandomInt = (min, max) => {
  return Math.floor(Math.random() * (max - min + 1) + min);
}
