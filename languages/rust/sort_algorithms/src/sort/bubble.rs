pub struct BubbleSort;

impl BubbleSort {
    pub fn new() -> Self {
        Self {}
    }
}

impl<T: PartialOrd + Clone> super::SortAlgorithm<T> for BubbleSort {
    fn sort(&self, data: &mut Vec<T>) {
        let mut change = true;
        while change {
            change = false;

            for n in 1..data.len() {
                if data[n] < data[n - 1] {
                    let tmp = data[n].clone();
                    data[n] = data[n - 1].clone();
                    data[n - 1] = tmp;
                    change = true;
                }
            }
        }
    }
}

impl super::Named for BubbleSort {
    fn get_name(&self) -> &str {
        "Bubble sort"
    }
}
