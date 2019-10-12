filename = 'C:\Users\ouoam\Desktop\Slides\Data Commu\file\MAGNIFICAT_16bits_96kHz.flac';
[y, Fs] = audioread(filename);
nBits = 24;
playerObj = audioplayer(y(:, 1), Fs, nBits);
get(playerObj);
play(playerObj);

% stop(playerObj);