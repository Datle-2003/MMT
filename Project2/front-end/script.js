const form = document.querySelector("form");
const submitButton = form.querySelector('input[type="submit"]');
const sortSelect = document.getElementById("sort");

function convertToProductArray(list) {
  return Array.from(list, function (li) {
    const name = li.querySelector("h3").textContent;
    const price = parseInt(li.querySelector("p").textContent);
    const link = li.querySelector("a").href;
    return { name, price, link };
  });
}

function updateProductList() {
  const productList = document.querySelector(".product-list");
  const productArray = convertToProductArray(
    productList.querySelectorAll("li")
  );
  return productArray;
}


function sortProducts(products, sortOption) {
  let sortedProducts;
  console.log("Sorting products:", products);
  switch (sortOption) {
    case "low-high":
      sortedProducts = products.sort((a, b) => a.price - b.price);
      console.log("Sorted products (low-high):", sortedProducts);
      return sortedProducts;
    case "high-low":
      sortedProducts = products.sort((a, b) => b.price - a.price);
      console.log("Sorted products (high-low):", sortedProducts);
      return sortedProducts;
    default:
      console.log("Unsorted products:", products);
      return products;
  }
}

function displayProducts(products) {
  if (products.length !== 0) {
    const sort = document.getElementById("sort-bar");
    sort.style.display = "block";
  } else {
    sort.style.display = "none";
  }
  const productList = document.querySelector(".product-list");
  productList.innerHTML = "";

  products.forEach(function (p) {
    const li = document.createElement("li");
    li.classList.add("product");

    const img = document.createElement("img");
    img.src = "laptop.jpg";
    img.alt = p.Name;
    li.appendChild(img);

    const h3 = document.createElement("h3");
    h3.textContent = p.name;
    li.appendChild(h3);

    const pTag = document.createElement("p");
    pTag.textContent = `${p.price}đ`;
    li.appendChild(pTag);

    const a = document.createElement("a");
    a.href = p.link;
    a.textContent = "Visit website";
    li.appendChild(a);

    productList.appendChild(li);
  });
}

sortSelect.addEventListener("change", () => {
  const productArray = updateProductList();
  sortValue = sortSelect.value;
  const sortedProducts = sortProducts(productArray, sortValue);
  displayProducts(sortedProducts);
});

function searchProducts(searchQuery, storeFilter, brandFilter, priceFilter) {
  fetch("http://localhost:5000", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({
      keyword: searchQuery,
      website_name: storeFilter,
      laptop_type: brandFilter,
      price: priceFilter,
    }),
  })
    .then((response) => response.json()) // parse response as JSON
    .then((data) => {
      console.log(data.items.results);
      displayProducts(data.items.results); // log response to console
    })
    .catch((error) => {
      console.error(error); // log any errors to console
    });
}

submitButton.addEventListener("click", function (e) {
  e.preventDefault(); // prevent the form from submitting
  // your code to store user input goes here
  const searchQuery = document.querySelector("#search-bar").value;
  const brandFilter = document.querySelector("#brand-filter").value;
  const storeFilter = document.querySelector("#store-filter").value;
  const priceFilter = document.querySelector("#price-filter").value;

  searchProducts(searchQuery, storeFilter, brandFilter, priceFilter);
});
