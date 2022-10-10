const path = require("path");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const tailwindcss = require("tailwindcss");

module.exports = {
  mode: "development",
  devServer: {
    https: false,
    static: {
      directory: path.join(__dirname, "public"),
    },
    headers: {
      "Access-Control-Allow-Origin": "*",
      "Access-Control-Allow-Methods": "GET, POST, PUT, DELETE, PATCH, OPTIONS",
      "Access-Control-Allow-Headers": "X-Requested-With, content-type, Authorization",
      "Cross-Origin-Opener-Policy": "same-origin",
      "Cross-Origin-Embedder-Policy": "require-corp",
    },
    compress: true,
    port: 3200,
  },
  devtool: "source-map",
  entry: path.resolve(__dirname, "src", "index.tsx"),
  // experiments: {
  //   asyncWebAssembly: true,
  // },
  output: {
    path: path.resolve(__dirname, "dist"),
    filename: "bundle.js",
    // webassemblyModuleFilename: "static/wasm/[modulehash].wasm",
  },
  resolve: {
    extensions: [".ts", ".tsx", ".js", ".jsx", ".data"],
    alias: {
      react: path.resolve("./node_modules/react"),
    },
  },
  externals: {
    fs: "empty",
  },
  module: {
    rules: [
      {
        include: [path.join(__dirname, "src")],
        test: /\.(js|jsx)$/,
        loader: "babel-loader",
        options: {
          presets: ["@babel/preset-react"],
        },
      },
      {
        test: /\.(ts|tsx)$/,
        loader: "babel-loader",
        exclude: /node_modules/,
        options: {
          presets: ["@babel/preset-typescript"],
        },
      },
      {
        test: /\.css$/i,
        include: path.resolve(__dirname, "src"),
        use: ["style-loader", "css-loader", "postcss-loader"],
      },
      // {
      //   test: /\.data/,
      //   use: "file-loader",
      //   type: "javascript/auto",
      //   exclude: /node_modules/,
      // },
      // {
      //   test: /\.worker\.js/,
      //   use: {
      //     loader: "worker-loader",
      //   },
      // },
      // {
      //   test: /\.(wasm)$/,
      //   loader: "file-loader",
      //   type: "javascript/auto",
      //   exclude: /node_modules/,
      // },
    ],
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: path.resolve(__dirname, "public", "index.html"),
      filename: "index.html",
      inject: "body",
    }),
  ],
};
