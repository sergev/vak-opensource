pub struct ShakerSort;

impl ShakerSort {
    pub fn new() -> Self {
        Self {}
    }
}

impl<T: PartialOrd + Clone> super::SortAlgorithm<T> for ShakerSort {
    fn sort(&self, data: &mut Vec<T>) {
        if data.len() == 0 {
            return
        }

        let mut left: usize = 0;
        let mut right = data.len() - 1;

        while left < right {
            for n in (left + 1)..(right + 1) {
                if data[n] < data[n - 1] {
                    let tmp = data[n].clone();
                    data[n] = data[n - 1].clone();
                    data[n - 1] = tmp;
                }
            }
            right -= 1;

            for n in (left..right).rev() {
                if data[n] > data[n + 1] {
                    let tmp = data[n].clone();
                    data[n] = data[n + 1].clone();
                    data[n + 1] = tmp;
                }
            }
            left += 1;
        }
    }
}

impl super::Named for ShakerSort {
    fn get_name(&self) -> &str {
        "Cocktail Shaker sort"
    }
}
